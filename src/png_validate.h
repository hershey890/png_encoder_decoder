/**
 * @file png_validate.h
 * @brief PNG file data validation functions
 */
#pragma once

#include "png_types_macros.h"

#ifdef __cplusplus
extern "C" {
#endif

int isChunkValid_IHDR(chunk_t* chunk);

#ifdef __cplusplus
}
#endif
