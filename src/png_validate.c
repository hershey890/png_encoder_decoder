#include "png_errors.h"
#include "png_validate.h"
#include "png_logger.h"

/**
 * @param data
 * @return 0 for no error, -ECHUNK_INVALID for error
 */
int isChunkValid_IHDR(chunk_t* chunk) {
    int err = 0;
    chunkIHDR_t* data = NULL;

    if(chunk->type != JOIN_BYTES_UINT32('I', 'H', 'D', 'R')) {
        err = -1;
        goto isChunkValid_IHDR_cleanup;
    }

    data = (chunkIHDR_t*)chunk->data;
    if(data->width > (1UL<<31)-1 || data->height > (1UL<<31)-1) {
        err = -2;
        goto isChunkValid_IHDR_cleanup;
    }
    if(data->bit_depth != BIT_DEPTH_1 && 
        data->bit_depth != BIT_DEPTH_2 && 
        data->bit_depth != BIT_DEPTH_4 && 
        data->bit_depth != BIT_DEPTH_8 && 
        data->bit_depth != BIT_DEPTH_16) 
    {
        err = -3;
        goto isChunkValid_IHDR_cleanup;
    }
    if(data->color_type != COLOR_0 && 
        data->color_type != COLOR_2 && 
        data->color_type != COLOR_3 && 
        data->color_type != COLOR_4 && 
        data->color_type != COLOR_6) 
    {
        err = -4;
        goto isChunkValid_IHDR_cleanup;
    }
    if(data->color_type == 0) {
        if(data->bit_depth != BIT_DEPTH_1 &&
            data->bit_depth != BIT_DEPTH_2 && 
            data->bit_depth != BIT_DEPTH_4 && 
            data->bit_depth != BIT_DEPTH_8 && 
            data->bit_depth != BIT_DEPTH_16) 
        {
            err = -5;
            goto isChunkValid_IHDR_cleanup;
        }
    }
    if(data->color_type == 2) {
        if(data->bit_depth != BIT_DEPTH_8 && data->bit_depth != BIT_DEPTH_16) {
            // TODO
        }
    }
    if(data->color_type == 3) {
        if(data->bit_depth != BIT_DEPTH_1 && 
            data->bit_depth != BIT_DEPTH_2 && 
            data->bit_depth != BIT_DEPTH_4 && 
            data->bit_depth != BIT_DEPTH_8) 
        {
            err = -6;
            goto isChunkValid_IHDR_cleanup;
        }
    }
    if(data->color_type == 4) {
        if(data->bit_depth != BIT_DEPTH_8 && data->bit_depth != BIT_DEPTH_16) {
            err = -7;
            goto isChunkValid_IHDR_cleanup;
        }
    }
    if(data->color_type == 6) {
        if(data->bit_depth != BIT_DEPTH_8 && data->bit_depth != BIT_DEPTH_16) {
            err = -8;
            goto isChunkValid_IHDR_cleanup;
        }
    }

isChunkValid_IHDR_cleanup:
    if (err) {
        PNG_LOGE("err point [%d]", err);
        printf("print error\n");
    }
    return err ? -ECHUNK_INVALID : 0;
}
