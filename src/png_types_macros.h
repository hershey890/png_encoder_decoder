#pragma once

#include <stdint.h>

#define REVERSE_UINT32(x) (  \
    ((x) & 0x000000ff) << 24 | \
    ((x) & 0x0000ff00) << 8  | \
    ((x) & 0x00ff0000) >> 8 | \
    ((x) & 0xff000000) >> 24)

static const int _n = 1;
#define REVERSE_UINT_IF_SYS_LITTLE_END(x) (*(char*)&_n == 1 ? REVERSE_UINT32((x)) : (x))

#define JOIN_BYTES_UINT32(a, b, c, d) ((a) | ((b) << 8) | ((c) << 16) | ((d) << 24))

typedef struct chunk_s {
    uint32_t length;
    uint32_t type;
    uint8_t* data;
    uint32_t crc;

} chunk_t;
