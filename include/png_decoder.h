#pragma once

/************************************************************************
 * INCLUDES
 ************************************************************************/
#include "png_errors.h"

#ifdef __cplusplus
extern "C" {
#endif

/************************************************************************
 * GLOBAL FUNCTION PROTOTYPES
 ************************************************************************/
/**
 * Decodes a PNG file
 * @param filename The PNG file to decode
 * @return 0 if successful, negative value for failure
 */
int decodePNG(const char* filename);


/************************************************************************
 * TEST FUNCTION EXPORTS
 ************************************************************************/
#ifdef GOOGLE_TEST

typedef struct chunk_s chunk_t;

struct _test_func_export_s {
    int (*readSignature)(FILE* file);
    int (*readChunk)(FILE* file, chunk_t* chunk);
};
#endif

#ifdef __cplusplus
}
#endif
