/************************************************************************
 * INCLUDES
 ************************************************************************/
#include "png_errors.h"
#include "png_logger.h"
#include "png_validate.h"

/************************************************************************
 * GLOBAL FUNCTIONS
 ************************************************************************/
int isChunkValid_IHDR(const image_t *p_img) {
    int err = 0;

    if (p_img->width > 0x7FFFFFFF || p_img->height > 0x7FFFFFFF) {
        err = 1;
        goto cleanup;
    }

    switch (p_img->bit_depth) {
        case BIT_DEPTH_1:
        case BIT_DEPTH_2:
        case BIT_DEPTH_4:
        case BIT_DEPTH_8:
        case BIT_DEPTH_16:
            break;
        default:
            err = 2;
            goto cleanup;
    }

    if (p_img->color_type == COLOR_Y) {
        ; /* Do nothing, all bit depths are valid */
    } else if (p_img->color_type == COLOR_RGB) {
        if (p_img->bit_depth != BIT_DEPTH_8 &&
            p_img->bit_depth != BIT_DEPTH_16) {
            err = 3;
            goto cleanup;
        }
    } else if (p_img->color_type == COLOR_PLTE) {
        if (p_img->bit_depth != BIT_DEPTH_1 &&
            p_img->bit_depth != BIT_DEPTH_2 &&
            p_img->bit_depth != BIT_DEPTH_4 &&
            p_img->bit_depth != BIT_DEPTH_8) {
            err = 4;
            goto cleanup;
        }

    } else if (p_img->color_type == COLOR_YA) {
        if (p_img->bit_depth != BIT_DEPTH_8 &&
            p_img->bit_depth != BIT_DEPTH_16) {
            err = 5;
            goto cleanup;
        }
    } else if (p_img->color_type == COLOR_RGBA) {
        if (p_img->bit_depth != BIT_DEPTH_8 &&
            p_img->bit_depth != BIT_DEPTH_16) {
            err = 6;
            goto cleanup;
        }
    } else {
        err = 7;
        goto cleanup;
    }

    if (p_img->compression_method != COMPRESSION_DEFLATE_INFLATE) {
        err = 8;
        goto cleanup;
    }

    if (p_img->filter_method != FILTER_ADAPTIVE) {
        err = 9;
        goto cleanup;
    }

cleanup:
    if (err) {
        PNG_LOGE("isChunkValid_IHDR: Failed at point %d\n", err);
        return -EIHDR;
    }
    return 0;
}