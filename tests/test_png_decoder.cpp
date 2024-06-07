#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <filesystem>
#include "../include/png_decoder.h"
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
    // Test valid signature
    uint8_t signature[] = {137, 80, 78, 71, 13, 10, 26, 10};
    file_ = fopen(filename_, "wb");
    fwrite(signature, sizeof(signature[0]), 8, file_);
    fclose(file_);

    int res = decodePNG(filename_);
    ASSERT_EQ(res, 0);

    // Test invalid signature
    signature[7] = 11;
    file_ = fopen(filename_, "wb");
    ASSERT_NE(file_, nullptr);
    fwrite(signature, sizeof(signature[0]), 8, file_);
    fclose(file_);
    
    res = decodePNG(filename_);
    ASSERT_EQ(res, -2);
}