#include <cstdio>
#include <cstdint>
#include <filesystem>
#include "../src/png_decoder.c"
#include <gtest/gtest.h>

class PNGDecoderTest : public testing::Test {
protected:
    FILE* file_;
    const char* filename_;

    PNGDecoderTest() : filename_(tmpnam(nullptr)) {}
    ~PNGDecoderTest() override {
        if(std::filesystem::exists(filename_))
            std::remove(filename_);
    }
};

TEST_F(PNGDecoderTest, ReadSignature) {
    // Test Valid input
    uint8_t signature[] = {137, 80, 78, 71, 13, 10, 26, 10};
    file_ = fopen(filename_, "wb");
    ASSERT_NE(file_, nullptr);
    fwrite(signature, sizeof(signature[0]), 8, file_);
    fclose(file_);
    
    file_ = fopen(filename_, "rb");
    int res = readSignature(file_);
    ASSERT_EQ(res, 0);
    
    // Test Invalid input
    signature[7] = 11;
    file_ = fopen(filename_, "wb");
    ASSERT_NE(file_, nullptr);
    fwrite(signature, sizeof(signature[0]), 8, file_);
    fclose(file_);
    
    file_ = fopen(filename_, "rb");
    ASSERT_NE(file_, nullptr);
    res = readSignature(file_);
    ASSERT_EQ(res, -ESIGNATURE);
    fclose(file_);
}

/* Table of CRCs of all 8-bit messages. */
unsigned long crc_table[256];

/* Flag: has the table been computed? Initially false. */
int crc_table_computed = 0;

/* Make the table for a fast CRC. */
void make_crc_table(void)
{
    unsigned long c;
    int n, k;

    for (n = 0; n < 256; n++) {
        c = (unsigned long) n;
        for (k = 0; k < 8; k++) {
            if (c & 1) {
                c = 0xedb88320L ^ (c >> 1);
            } else {
                c = c >> 1;
            }
        }
        crc_table[n] = c;
    }
    crc_table_computed = 1;
}

/* Update a running CRC with the bytes buf[0..len-1]--the CRC
    should be initialized to all 1's, and the transmitted value
    is the 1's complement of the final running CRC (see the
    crc() routine below)). */

unsigned long update_crc(unsigned long crc, unsigned char *buf,
                        int len)
{
    unsigned long c = crc;
    int n;

    if (!crc_table_computed) {
        make_crc_table();
    }
    for (n = 0; n < len; n++) {
        c = crc_table[(c ^ buf[n]) & 0xff] ^ (c >> 8);
    }
    return c;
}

/* Return the CRC of the bytes buf[0..len-1]. */
unsigned long crc(unsigned char *buf, int len)
{
    return update_crc(0xffffffffL, buf, len) ^ 0xffffffffL;
}

TEST_F(PNGDecoderTest, ReadChunk) {
    file_ = fopen(filename_, "wb");
    ASSERT_NE(file_, nullptr);
    uint8_t file_chunk[] = {
        0x00, 0x00, 0x00, 0x0d, // length
        'I', 'H', 'D', 'R', // 4 bytes for chunk type, this one is IHDR. Backward bc in network byte order i.e. big endian
        0x00, 0x00, 0x09, 0x4c, 0x00, 0x00, 0x09, 0x4f, 0x08, 0x02, 0x00, 0x00, 0x00,  // any # bytes for chunk data
        0x20, 0xb4, 0xcf, 0x84 // 4 bytes for CRC
    };
    const size_t chunk_len = sizeof(file_chunk);
    fwrite(file_chunk, sizeof(file_chunk[0]), sizeof(file_chunk), file_);
    fclose(file_);

    file_ = fopen(filename_, "rb");
    chunk_t chunk;
    int res = readChunk(file_, &chunk);
    fclose(file_);

    for(size_t i=0; i<chunk.length; i++) {
        ASSERT_EQ(chunk.data[i], file_chunk[8+i]);
    }
    free(chunk.data);
    ASSERT_EQ(chunk.length, chunk_len - 12);
    ASSERT_EQ(chunk.type, JOIN_BYTES_UINT32('I', 'H', 'D', 'R'));
    ASSERT_EQ(chunk.crc, *(uint32_t*)(&file_chunk[chunk_len - 4]));
    uint32_t crc_val = crc(&file_chunk[4], chunk_len - 8);
    crc_val = REVERSE_UINT_IF_SYS_LITTLE_END(crc_val);
    ASSERT_EQ(chunk.crc, crc_val);
    ASSERT_EQ(res, 0);
}

// TEST_F(PNGDecoderTest, DecodePNG) {
//     // Test valid signature
//     uint8_t signature[] = {137, 80, 78, 71, 13, 10, 26, 10};
//     file_ = fopen(filename_, "wb");
//     fwrite(signature, sizeof(signature[0]), 8, file_);
//     fclose(file_);

//     int res = decodePNG(filename_);
//     ASSERT_EQ(res, 0);

//     // Test invalid signature
//     signature[7] = 11;
//     file_ = fopen(filename_, "wb");
//     ASSERT_NE(file_, nullptr);
//     fwrite(signature, sizeof(signature[0]), 8, file_);
//     fclose(file_);
    
//     res = decodePNG(filename_);
//     ASSERT_EQ(res, -2);
// }