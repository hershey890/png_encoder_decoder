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


/* Table of CRCs of all 8-bit messages. */
static unsigned long crc_table[256];
static int crc_table_computed = 0;

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

/* Make the table for a fast CRC. */
static void make_crc_table(void) {
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
static unsigned long update_crc(unsigned long crc, unsigned char *buf,
                        int len) {
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
__attribute_maybe_unused__ static unsigned long crc(unsigned char *buf, int len) {
    return update_crc(0xffffffffL, buf, len) ^ 0xffffffffL;
}

/**
 * Reads a PNG chunk into `data`. Be sure to free `chunk->data` after usage is complete
 * @param chunk
 * @return 0 if successful, -ECHUNK if fails
 */
static int readChunk(FILE* file, chunk_t* chunk) {
    int err = 0;
    chunk->data = NULL;

    // Chunk: 4 bytes for length, 4 bytes for chunk type, chunk data, 4 bytes for crc
    if(fread((void*)&chunk->length, sizeof(chunk->length), 1, file) != 1) {
        err = 1;
        goto cleanup;
    }
    chunk->length = REVERSE_UINT32_IF_SYS_LITTLE_END(chunk->length);

    if(fread((void*)&chunk->type, sizeof(chunk->type), 1, file) != 1) {
        err = 2;
        goto cleanup;
    }

    if(chunk->length == 0) {
        chunk->data = NULL;
        return 0;
    }

    chunk->data = (uint8_t*)malloc(chunk->length);
    if(chunk->data == NULL) {
        err = 3;
        goto cleanup;
    }
    if(fread(chunk->data, 1, chunk->length, file) != chunk->length) {
        err = 4;
        goto cleanup;
    }

    if(fread((void*)&chunk->crc, sizeof(chunk->crc), 1, file) != 1) {
        err = 5;
        goto cleanup;
    }

    /* Validate CRC */
    uint8_t type_buf[4] = {
        (uint8_t)(chunk->type & 0xFF),
        (uint8_t)((chunk->type >> 8) & 0xFF),
        (uint8_t)((chunk->type >> 16) & 0xFF),
        (uint8_t)((chunk->type >> 24) & 0xFF),
    };
    unsigned long calculated_crc = update_crc(0xffffffffL, type_buf, 4);
    calculated_crc = update_crc(calculated_crc, chunk->data, chunk->length);
    calculated_crc ^= 0xffffffffL;
    calculated_crc = REVERSE_UINT32_IF_SYS_LITTLE_END(calculated_crc);

    if (calculated_crc != chunk->crc) {
        err = 6;
        goto cleanup;
    }

cleanup:
    if (err) {
        PNG_LOGE("readChunk: Failed at point %d\n", err);
        return -ECHUNK;
    }
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
