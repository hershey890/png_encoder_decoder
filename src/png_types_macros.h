#pragma once

/************************************************************************
 * INCLUDES
 ************************************************************************/
#include <stdint.h>


/************************************************************************
 * MACROS AND DEFINES
 ************************************************************************/
#define REVERSE_UINT32(x) (  \
    ((x) & 0x000000ff) << 24 | \
    ((x) & 0x0000ff00) << 8  | \
    ((x) & 0x00ff0000) >> 8 | \
    ((x) & 0xff000000) >> 24)

static const int _n = 1;
#define REVERSE_UINT32_IF_SYS_LITTLE_END(x) (*(char*)&_n == 1 ? REVERSE_UINT32((x)) : (x))

#define JOIN_BYTES_UINT32(a, b, c, d) ((a) | ((b) << 8) | ((c) << 16) | ((d) << 24))


/************************************************************************
 * TYPEDEFS
 ************************************************************************/
typedef struct chunk_s {
    uint32_t length;
    uint32_t type;
    uint8_t* data;
    uint32_t crc;

} chunk_t;

typedef enum {
    BIT_DEPTH_1  = 1,
    BIT_DEPTH_2  = 2,
    BIT_DEPTH_4  = 4,
    BIT_DEPTH_8  = 8,
    BIT_DEPTH_16 = 16,
} bit_depth_t;

typedef enum {
    COLOR_PALATTE_USED = 1,
    COLOR_USED = 2,
    COLOR_ALPHA_CHANNEL_USED = 3,
} color_configs_t;

/**Represent valid sums of color_config_t values */
typedef enum {
    COLOR_0 = 0,
    COLOR_2 = 2,
    COLOR_3 = 3,
    COLOR_4 = 4,
    COLOR_6 = 6,
} color_type_t;

typedef struct __attribute__((packed)) chunkIHDR_s {
    uint32_t width;             // max 2^31-1
    uint32_t height;            // max 2^31-1
    uint8_t bit_depth;          // valid values 1, 2, 4, 8, 16
    uint8_t color_type;         // valid values 0, 2, 3, 4, 6
    uint8_t compression_method; // 
    uint8_t filter_method;
    uint8_t interlace_method;
} chunkIHDR_t;
