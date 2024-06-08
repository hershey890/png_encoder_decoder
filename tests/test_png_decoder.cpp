#include <cstdio>
#include <cstdint>
#include <filesystem>
#include "../src/png_decoder.c"
#include <gtest/gtest.h>

class PNGDecoderTest : public testing::Test {
protected:
    FILE* file_;
    const char* filename_;

    PNGDecoderTest() : filename_(tmpnam(NULL)) {}
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
    ASSERT_EQ(res, -2);
    fclose(file_);
}

TEST_F(PNGDecoderTest, ReadChunk) {
    file_ = fopen(filename_, "wb");
    ASSERT_NE(file_, nullptr);
    uint8_t signature[] = {137, 80, 78, 71, 13, 10, 26, 10};
    fwrite(signature, sizeof(signature[0]), 8, file_);

    uint8_t chunk[] = {
        0x00, 0x00, 0x00, 0x02, // length
        73, 72, 68, 82, // 4 bytes for chunk type, this one is IHDR
        0x13, 0xE2, // any # bytes for chunk data
        // crc
    };
    fclose(file_);

    uint32_t length;
    uint32_t type;
    uint8_t *data;
    uint32_t crc;
    int res = readChunk(file_, &length, &type, &data, &crc);
    ASSERT_EQ(res, 0);
    fclose(file_);
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