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
 * LOCAL VARIABLES
 ************************************************************************/
/* Table of CRCs of all 8-bit messages. */
static unsigned long crc_table[256];
static int crc_table_computed = 0;

/************************************************************************
 * STATIC FUNCTIONS
 ************************************************************************/
/**
 * Reads the PNG file signature to verify file is a PNG file
 * @param file[in] The PNG file pointer
 * @return 0 if successful, -ESIGNATURE if fails
 */
static int readSignature(FILE* file) {
    #define SIGNATURE_SIZE 8

    if (file == NULL) {
        PNG_LOGE("readSignature: File pointer is NULL\n");
        return -ESIGNATURE;
    }

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
static unsigned long update_crc(unsigned long crc, unsigned char *p_buf,
                        int len) {
    unsigned long c = crc;
    int n;

    if (!crc_table_computed) {
        make_crc_table();
    }
    for (n = 0; n < len; n++) {
        c = crc_table[(c ^ p_buf[n]) & 0xff] ^ (c >> 8);
    }
    return c;
}

/* Return the CRC of the bytes buf[0..len-1]. */
__attribute_maybe_unused__ 
static unsigned long crc(unsigned char *p_buf, int len) {
    return update_crc(0xffffffffL, p_buf, len) ^ 0xffffffffL;
}

/**
 * Reads a PNG chunk into `data`. Be sure to free `p_chunk->data` after usage is complete
 * @param file[in]
 * @param p_chunk[out]
 * @return 0 if successful, -ECHUNK if fails
 */
static int readChunk(FILE* file, chunk_t* p_chunk) {
    int err = 0;

    if (file == NULL || p_chunk == NULL) {
        err = 1;
        goto cleanup;
    }
    
    p_chunk->data = NULL;

    // Chunk: 4 bytes for length, 4 bytes for chunk type, chunk data, 4 bytes for crc
    if(fread((void*)&p_chunk->length, sizeof(p_chunk->length), 1, file) != 1) {
        err = 2;
        goto cleanup;
    }
    p_chunk->length = REVERSE_UINT32_IF_SYS_LITTLE_END(p_chunk->length);

    if(fread((void*)&p_chunk->type, sizeof(p_chunk->type), 1, file) != 1) {
        err = 3;
        goto cleanup;
    }

    if(p_chunk->length == 0) {
        p_chunk->data = NULL;
        return 0;
    }

    p_chunk->data = (uint8_t*)malloc(p_chunk->length);
    if(p_chunk->data == NULL) {
        err = 4;
        goto cleanup;
    }
    if(fread(p_chunk->data, 1, p_chunk->length, file) != p_chunk->length) {
        err = 5;
        goto cleanup;
    }

    if(fread((void*)&p_chunk->crc, sizeof(p_chunk->crc), 1, file) != 1) {
        err = 6;
        goto cleanup;
    }

    /* Validate CRC */
    uint8_t type_buf[4] = {
        (uint8_t)(p_chunk->type & 0xFF),
        (uint8_t)((p_chunk->type >> 8) & 0xFF),
        (uint8_t)((p_chunk->type >> 16) & 0xFF),
        (uint8_t)((p_chunk->type >> 24) & 0xFF),
    };
    unsigned long calculated_crc = update_crc(0xffffffffL, type_buf, 4);
    calculated_crc = update_crc(calculated_crc, p_chunk->data, p_chunk->length);
    calculated_crc ^= 0xffffffffL;
    calculated_crc = REVERSE_UINT32_IF_SYS_LITTLE_END(calculated_crc);

    if (calculated_crc != p_chunk->crc) {
        err = 7;
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
 * @param p_chunk[in] The IHDR chunk
 * @param p_img[out]
 * @return 0 if successful, -3 if failed
 */
__attribute_maybe_unused__
static int readIHDR(chunk_t *p_chunk, image_t *p_img) {
    int err = 0;

    if (p_chunk == NULL || p_img == NULL || p_chunk->data == NULL) {
        err = 1;
        goto cleanup;
    }
    if (p_chunk->length != sizeof(chunkIHDR_t)) {
        err = 2;
        goto cleanup;
    }

    chunkIHDR_t* p_packed_ihdr = (chunkIHDR_t*)p_chunk->data;
    p_img->width              = REVERSE_UINT32_IF_SYS_LITTLE_END(p_packed_ihdr->width);
    p_img->height             = REVERSE_UINT32_IF_SYS_LITTLE_END(p_packed_ihdr->height);
    p_img->bit_depth          = p_packed_ihdr->bit_depth;
    p_img->color_type         = p_packed_ihdr->color_type;
    p_img->compression_method = p_packed_ihdr->compression_method;
    p_img->filter_method      = p_packed_ihdr->filter_method;
    p_img->interlace_method   = p_packed_ihdr->interlace_method;
    p_img->sample_depth       = p_img->color_type == COLOR_PLTE ? 
                                    BIT_DEPTH_8 : p_img->bit_depth;

    int res = isChunkValid_IHDR(p_img);
    if (res != 0) {
        err = 2 + res;
        goto cleanup;
    }

cleanup:
    if (err) {
        PNG_LOGE("readIHDR: Failed at point %d\n", err);
        return -EIHDR;
    }
    return 0;
}

// readIDAT

// readIEND

/************************************************************************
 * GLOBAL FUNCTIONS
 ************************************************************************/
int decodePNG(const char* filename) {
    int res = 0;
    FILE *file = fopen(filename, "rb");
    if(file == NULL) {
        return -EFILE;
    }
    
    if((res = readSignature(file)) != 0) {
        fclose(file);
        return res;
    }

    chunk_t chunk;
    if((res = readChunk(file, &chunk)) != 0) {
        if (chunk.data) {
            free(chunk.data);
        }
        fclose(file);
        return res;
    }

    // switch (chunk.type) {
    //     case JOIN_BYTES_UINT32('I', 'H', 'D', 'R'):
    //         readIHDR(chunk.data, chunk.length);
    //         break;
    //     case JOIN_BYTES_UINT32('P', 'L', 'T', 'E'):
    //     case JOIN_BYTES_UINT32('I', 'D', 'A', 'T'):
    //     case JOIN_BYTES_UINT32('I', 'E', 'N', 'D'):
    //     case JOIN_BYTES_UINT32('c', 'H', 'R', 'M'):
    //     case JOIN_BYTES_UINT32('g', 'A', 'M', 'A'):
    //     case JOIN_BYTES_UINT32('i', 'C', 'C', 'P'):
    //     case JOIN_BYTES_UINT32('s', 'B', 'I', 'T'):
    //     case JOIN_BYTES_UINT32('s', 'R', 'G', 'B'):
    //     case JOIN_BYTES_UINT32('b', 'K', 'G', 'D'):
    //     case JOIN_BYTES_UINT32('h', 'I', 'S', 'T'):
    //     case JOIN_BYTES_UINT32('t', 'R', 'N', 'S'):
    //     case JOIN_BYTES_UINT32('p', 'H', 'Y', 's'):
    //     case JOIN_BYTES_UINT32('s', 'P', 'L', 'T'):
    //     case JOIN_BYTES_UINT32('t', 'I', 'M', 'E'):
    //     case JOIN_BYTES_UINT32('i', 'T', 'X', 't'):
    //     case JOIN_BYTES_UINT32('t', 'E', 'X', 't'):
    //     case JOIN_BYTES_UINT32('z', 'T', 'X', 't'):
    //     default:
    //         PNG_LOGW("decodePNG: Unknown chunk type: 0x%08X\n", chunk.type);
    //         break;
    // }

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
