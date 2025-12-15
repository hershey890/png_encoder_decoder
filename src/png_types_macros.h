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
    
#define CHUNK_IHDR JOIN_BYTES_UINT32('I', 'H', 'D', 'R')
#define CHUNK_PLTE JOIN_BYTES_UINT32('P', 'L', 'T', 'E')
#define CHUNK_IDAT JOIN_BYTES_UINT32('I', 'D', 'A', 'T')
#define CHUNK_IEND JOIN_BYTES_UINT32('I', 'E', 'N', 'D')
#define CHUNK_cHRM JOIN_BYTES_UINT32('c', 'H', 'R', 'M')
#define CHUNK_gAMA JOIN_BYTES_UINT32('g', 'A', 'M', 'A')
#define CHUNK_iCCP JOIN_BYTES_UINT32('i', 'C', 'C', 'P')
#define CHUNK_sBIT JOIN_BYTES_UINT32('s', 'B', 'I', 'T')
#define CHUNK_sRGB JOIN_BYTES_UINT32('s', 'R', 'G', 'B')
#define CHUNK_bKGD JOIN_BYTES_UINT32('b', 'K', 'G', 'D')
#define CHUNK_hIST JOIN_BYTES_UINT32('h', 'I', 'S', 'T')
#define CHUNK_tRNS JOIN_BYTES_UINT32('t', 'R', 'N', 'S')
#define CHUNK_pHYs JOIN_BYTES_UINT32('p', 'H', 'Y', 's')
#define CHUNK_sPLT JOIN_BYTES_UINT32('s', 'P', 'L', 'T')
#define CHUNK_tIME JOIN_BYTES_UINT32('t', 'I', 'M', 'E')
#define CHUNK_iTXt JOIN_BYTES_UINT32('i', 'T', 'X', 't')
#define CHUNK_tEXt JOIN_BYTES_UINT32('t', 'E', 'X', 't')
#define CHUNK_zTXt JOIN_BYTES_UINT32('z', 'T', 'X', 't')


/************************************************************************
 * TYPEDEFS
 ************************************************************************/
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
    COLOR_Y    = 0, // Grayscale
    COLOR_RGB  = 2,
    COLOR_PLTE = 3, // Each pixel is a palette index. PLTE chunk required
    COLOR_YA   = 4, // Grayscale with alpha
    COLOR_RGBA = 6,
} color_type_t;

typedef enum {
    COMPRESSION_DEFLATE_INFLATE = 0,
} compression_method_t;

typedef enum {
    FILTER_ADAPTIVE = 0,
} filter_method_t;

typedef enum {
    INTERLACE_NONE    = 0,
    INTERLACE_ADAM7  = 1,
} interlace_method_t;

typedef struct chunk_s {
    uint32_t length;
    uint32_t type; /* 4 chars are packed reversed */
    uint8_t* data;
    uint32_t crc;
} chunk_t;

typedef struct __attribute__((packed)) chunkIHDR_s {
    uint32_t width;
    uint32_t height;
    uint8_t bit_depth;
    uint8_t color_type;
    uint8_t compression_method;
    uint8_t filter_method;
    uint8_t interlace_method;
} chunkIHDR_t;

typedef struct image_s {
    uint32_t width;                         // max 2^31-1
    uint32_t height;                        // max 2^31-1
    bit_depth_t bit_depth;                  // valid values 1, 2, 4, 8, 16
    color_type_t color_type;                // valid values 0, 2, 3, 4, 6
    compression_method_t compression_method;// De/inflate w/ sliding window up to 32768 bytes
    uint8_t filter_method;                  // Adaptive filtering with five basic filter types  
    uint8_t interlace_method;
    uint8_t sample_depth;                   // Same as bit depth unless color_type is 3, then 8
} image_t;

// typedef enum {
//     CHUNK_CRITICAL_MIN  = 0,
//     CHUNK_IHDR = 0,
//     CHUNK_PLTE = 1,
//     CHUNK_IDAT = 2,
//     CHUNK_IEND = 3,
//     CHUNK_CRITICAL_MAX = 3,
//     CHUNK_ANCIILLARY_MIN = 4,
//     CHUNK_cHRM = 4,
//     CHUNK_gAMA = 5,
//     CHUNK_iCCP = 6,
//     CHUNK_sBIT = 7,
//     CHUNK_sRGB = 8,
//     CHUNK_bKGD = 9,
//     CHUNK_hIST = 10,
//     CHUNK_tRNS = 11,
//     CHUNK_pHYs = 12,
//     CHUNK_sPLT = 13,
//     CHUNK_tIME = 14,
//     CHUNK_iTXt = 15,
//     CHUNK_tEXt = 16,
//     CHUNK_zTXt = 17,
//     CHUNK_ANCILLARY_MAX = 17,
// } chunk_type_enum_t;

// const uint32_t chunk_type[] = {
//     JOIN_BYTES_UINT32('I', 'H', 'D', 'R'),
//     JOIN_BYTES_UINT32('P', 'L', 'T', 'E'),
//     JOIN_BYTES_UINT32('I', 'D', 'A', 'T'),
//     JOIN_BYTES_UINT32('I', 'E', 'N', 'D'),
//     JOIN_BYTES_UINT32('c', 'H', 'R', 'M'),
//     JOIN_BYTES_UINT32('g', 'A', 'M', 'A'),
//     JOIN_BYTES_UINT32('i', 'C', 'C', 'P'),
//     JOIN_BYTES_UINT32('s', 'B', 'I', 'T'),
//     JOIN_BYTES_UINT32('s', 'R', 'G', 'B'),
//     JOIN_BYTES_UINT32('b', 'K', 'G', 'D'),
//     JOIN_BYTES_UINT32('h', 'I', 'S', 'T'),
//     JOIN_BYTES_UINT32('t', 'R', 'N', 'S'),
//     JOIN_BYTES_UINT32('p', 'H', 'Y', 's'),
//     JOIN_BYTES_UINT32('s', 'P', 'L', 'T'),
//     JOIN_BYTES_UINT32('t', 'I', 'M', 'E'),
//     JOIN_BYTES_UINT32('i', 'T', 'X', 't'),
//     JOIN_BYTES_UINT32('t', 'E', 'X', 't'),
//     JOIN_BYTES_UINT32('z', 'T', 'X', 't'),
// };
