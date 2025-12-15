/**
 * @file png_validate.h
 * @brief PNG file data validation functions
 */
#pragma once

/************************************************************************
 * INCLUDES
 ************************************************************************/
#include "png_types_macros.h"

#ifdef __cplusplus
extern "C" {
#endif

/************************************************************************
 * GLOBAL FUNCTION DECLARATIONS
 ************************************************************************/
/**
 * @param p_img[in] Uses the fields from chunkIHDR_t
 * @return 0 for no error, integer > 0 indicating error point
 */
int isChunkValid_IHDR(const image_t *p_img);

#ifdef __cplusplus
}
#endif
