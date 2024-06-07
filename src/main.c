/**
 * For testing purposes
 * @file main.c
 * @date 2024-06-06
 * @brief Main file for testing the PNG decoder
 * @version 1.0
 */
#include <stdio.h>
#include "png_decoder.h"

int main(void) {
    const char* filename = "images/test.png";
    int res = decodePNG(filename);
    if(res != 0) {
        printf("Failed to decode PNG file: %s, error: %d\n", filename, res);
        return res;
    }

    printf("Successfully decoded PNG file %s\n", filename);
    return 0;
}