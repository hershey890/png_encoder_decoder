#include <cstdio>
#include <cstdint>
#include <filesystem>
#include <gtest/gtest.h>
#include "png_errors.h"
#include "png_decoder.h"
#include "png_validate.h"
#include "png_types_macros.h"

extern struct _test_func_export_s _test_func_export;

int readSignature(FILE* file) {
    return (*_test_func_export.readSignature)(file);
}

int readChunk(FILE* file, chunk_t* chunk) {
    return (*_test_func_export.readChunk)(file, chunk);
}

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

/**
 * Data pulled from a real png file via a chunk inspector
 */
TEST_F(PNGDecoderTest, ReadChunk) 
{
    /* Create test file with only IHDR chunk*/
    file_ = fopen(filename_, "wb");
    ASSERT_NE(file_, nullptr);
    uint8_t file_chunk[] = {
        0x00, 0x00, 0x00, 0x0d, 
        0x49, 0x48, 0x44, 0x52, // 4 bytes for chunk type, this one is IHDR. Backward bc in network byte order i.e. big endian
        0x00, 0x00, 0x09, 0x4c, 0x00, 0x00, 0x09, 0x4f, 0x08, 0x02, 0x00, 0x00, 0x00, 0x20, 0xb4, 0xcf, 0x84, // Chunk data
        0x20, 0xb4, 0xcf, 0x84 // 4 bytes for CRC
    };
    fwrite(file_chunk, sizeof(file_chunk[0]), sizeof(file_chunk), file_);
    fclose(file_);

    /* Read test file and parse IHDR chunk */
    file_ = fopen(filename_, "rb");
    chunk_t chunk;
    ASSERT_EQ(readChunk(file_, &chunk), 0);
    fclose(file_);

    /* Check Chunk Data */
    ASSERT_EQ(chunk.length, 13);
    ASSERT_EQ(chunk.type, JOIN_BYTES_UINT32('I', 'H', 'D', 'R'));
    for(size_t i=0; i<chunk.length; i++) {
        EXPECT_EQ(chunk.data[i], file_chunk[8+i]);
    }
    ASSERT_EQ(chunk.crc, 0x84cfb420);
    /* Check these values
    typedef struct __attribute__((packed)) chunkIHDR_s {
        uint32_t width;             // max 2^31-1
        uint32_t height;            // max 2^31-1
        uint8_t bit_depth;          // valid values 1, 2, 4, 8, 16
        uint8_t color_type;         // valid values 0, 2, 3, 4, 6
        uint8_t compression_method; // 
        uint8_t filter_method;
        uint8_t interlace_method;
    } chunkIHDR_t;
    */


    ASSERT_EQ(isChunkValid_IHDR(&chunk), 0); // TODO: byte flipping for datettings
    
    free(chunk.data);
}

// // TEST_F(PNGDecoderTest, DecodePNG) {
// //     // Test valid signature
// //     uint8_t signature[] = {137, 80, 78, 71, 13, 10, 26, 10};
// //     file_ = fopen(filename_, "wb");
// //     fwrite(signature, sizeof(signature[0]), 8, file_);
// //     fclose(file_);

// //     int res = decodePNG(filename_);
// //     ASSERT_EQ(res, 0);

// //     // Test invalid signature
// //     signature[7] = 11;
// //     file_ = fopen(filename_, "wb");
// //     ASSERT_NE(file_, nullptr);
// //     fwrite(signature, sizeof(signature[0]), 8, file_);
// //     fclose(file_);
    
// //     res = decodePNG(filename_);
// //     ASSERT_EQ(res, -2);
// // }