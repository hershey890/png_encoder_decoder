#ifndef GOOGLE_TEST
    #define STATIC static
#else
    #define STATIC
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "png_decoder.h"

/**
 * Reads the PNG file signature to verify file is a PNG file
 * @param file The PNG file pointer
 * @return 0 if successful, -2 if failed
 */
STATIC int readSignature(FILE* file) {
    #define SIGNATURE_SIZE 8
    const uint8_t expected_signature[SIGNATURE_SIZE] = {
        137, 80, 78, 71, 13, 10, 26, 10
    };
    uint8_t signature[SIGNATURE_SIZE];
    if(fread(signature, sizeof(signature[0]), SIGNATURE_SIZE, file) != SIGNATURE_SIZE)
        return -2;

    for(size_t i=0; i<SIGNATURE_SIZE; i++) {
        if(signature[i] != expected_signature[i])
            return -2;
    }

    return 0;
    #undef SIGNATURE_SIZE
}

/**
 * Reads a PNG chunk
 * @param file The PNG file pointer
 * @return 0 if successful, -3 if failed
 */
STATIC int readChunk(FILE* file, uint32_t* length, uint32_t* type, uint8_t** data, uint32_t* crc) {
    // Read length
    if(fread(length, sizeof(length), 1, file) != 1)
        return -3;

    // Read type
    if(fread(type, sizeof(type), 1, file) != 1)
        return -3;

    // Read data
    if(*length == 0) {
        *data = NULL;
        return 0;
    }
    if((*data = (uint8_t*)malloc(*length)) == NULL)
        return -3;
    if(fread(*data, 1, *length, file) != *length)
        return -3;

    // Read CRC
    if(fread(crc, sizeof(crc), 1, file) != 1)
        return -3;
    // TODO: actually check the CRC. The spec contains code for checking the CRC

    return 0;
}

/**
 * Reads the PNG file IHDR chunk/image header
 * @param file The PNG file pointer
 * @return 0 if successful, -3 if failed
 */
// STATIC int readIHDR(FILE* file) {
//     // Read the IHDR chunk
//     const size_t IHDR_SIZE = 13;
//     uint8_t ihdr[IHDR_SIZE];
//     if(fgets(ihdr, IHDR_SIZE, file) == NULL)
//         return -3;
    

//     return 0;
// }

// readIDAT

// readIEND

int decodePNG(const char* filename) {
    int res = 0;
    FILE *file = fopen(filename, "rb");
    if(file == NULL)
        return -1;
    
    if((res = readSignature(file)) != 0) {
        fclose(file);
        return res;
    }

    // uint32_t length;
    // uint32_t type;
    // uint8_t *data;
    // uint32_t crc;
    // if((res = readChunk(file, &length, &type, &data, &crc)) != 0) {
    //     fclose(file);
    //     return res;
    // }

    fclose(file);
    return 0;
}
