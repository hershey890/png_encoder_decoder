#define PNG_LOG_ERROR


/************************************************************************
 * INCLUDES
 ************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "png_logger.h"
#include "png_errors.h"
#include "png_decoder.h"
#include "png_validate.h"
#include "png_types_macros.h"


/************************************************************************
 * STATIC FUNCTIONS
 ************************************************************************/
/**
 * Reads the PNG file signature to verify file is a PNG file
 * @param file The PNG file pointer
 * @return 0 if successful, -ESIGNATURE if fails
 */
static int readSignature(FILE* file) {
    #define SIGNATURE_SIZE 8
    const uint8_t expected_signature[SIGNATURE_SIZE] = {
        137, 80, 78, 71, 13, 10, 26, 10
    };
    uint8_t signature[SIGNATURE_SIZE];
    if(fread(signature, sizeof(signature[0]), SIGNATURE_SIZE, file) != SIGNATURE_SIZE) {
        PNG_LOGE("File too short. Unable to read signature bytes\n");
        return -ESIGNATURE;
    }

    for(size_t i=0; i<SIGNATURE_SIZE; i++) {
        if(signature[i] != expected_signature[i]) {
            PNG_LOGE("Not a .png file. Invalid file PNG file signature\n");
            return -ESIGNATURE;
        }
    }

    return 0;
    #undef SIGNATURE_SIZE
}

/**
 * Reads a PNG chunk into `data`. Be sure to free `chunk->data` after usage is complete
 * @param chunk
 * @return 0 if successful, -ECHUNK if fails
 */
static int readChunk(FILE* file, chunk_t* chunk) {
    // Chunk: 4 bytes for length, 4 bytes for chunk type, chunk data, 4 bytes for crc
    if(fread((void*)&chunk->length, sizeof(chunk->length), 1, file) != 1) {
        PNG_LOGE("File too short. Unable to read chunk data bytes");
        return -ECHUNK;
    }
    chunk->length = REVERSE_UINT32_IF_SYS_LITTLE_END(chunk->length);

    if(fread((void*)&chunk->type, sizeof(chunk->type), 1, file) != 1) {
        PNG_LOGE("readChunk: fread error - Unable to read chunk type.");
        return -ECHUNK;
    }

    if(chunk->length == 0) {
        chunk->data = NULL;
        return 0;
    }

    chunk->data = (uint8_t*)malloc(chunk->length);
    if(chunk->data == NULL) {
        PNG_LOGE("readChunk: buffer malloc failed\n");
        return -ECHUNK;
    }
    if(fread(chunk->data, 1, chunk->length, file) != chunk->length) {
        PNG_LOGE("readChunk: fread error - reading chunk failed\n");
        return -ECHUNK;
    }

    if(fread((void*)&chunk->crc, sizeof(chunk->crc), 1, file) != 1) {
        PNG_LOGE("readChunk: fread error - CRC\n");
        return -ECHUNK;
    }

    // TODO: validate that the CRC is correct

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


/************************************************************************
 * GLOBAL FUNCTIONS
 ************************************************************************/
int decodePNG(const char* filename) {
    int res = 0;
    FILE *file = fopen(filename, "rb");
    if(file == NULL) {
        return -1;
    }
    
    if((res = readSignature(file)) != 0) {
        fclose(file);
        return res;
    }

    chunk_t chunk;
    if((res = readChunk(file, &chunk)) != 0) {
        free(chunk.data);
        fclose(file);
        return res;
    }
    free(chunk.data);
    

    fclose(file);
    return 0;
}


/************************************************************************
 * TEST FUNCTION EXPORTS
 ************************************************************************/
#ifdef GOOGLE_TEST

struct _test_func_export_s _test_func_export = {
    .readSignature = readSignature,
    .readChunk = readChunk,
};

#endif
