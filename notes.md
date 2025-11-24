## Building and Running
For CMake users:
```
$ cmake -S . -B build -Wno-dev
$ cmake --build build
$ build/main.exe
```
The executables will now available in `./build/{sub project name}/`. No manual file download is needed for GoogleTest is needed, the CMake script pulls everything in.

If you do not use Cmake:
```
$ gcc -Iinclude -o main.exe src/png_decoder.c src/main.c
```

## Debugging
```
$ cmake -S . -B build -Wno-dev -DCMAKE_BUILD_TYPE=Debug
$ cmake --build build
$ build/main.exe
```

## Testing
(Based on [this GoogleTest guide](https://google.github.io/googletest/quickstart-cmake.html)). To build and run the tests, use the following:  
```
$ cmake -S . -B build -Wno-dev
$ cmake --build build
$ (cd build && ctest --output-on-failure)
```

## Data Representation
- All integers with > 1 byte are in network byte order: the most significant byte comes first, then the less significant bytes
- values are typically unsigned. Signed values are in 2's comp
- Signed values are limited to [0,2^31] and unsigned are [-2^31-1,2^31]
- Colors range from [0,2^sampledepth] with 0 being black
- Color may be RGB or grayscale with grayscale.
- Pixels are typically ordered left-to-right within a scanline and scanlines top to bottom
    - there are exceptions ex. interlaced data order
- Size of pixel: bit depth which is the # of bits/sample
- pixel types:
    - indexed color: pixel is an index to a supplied color palette. Bit depth determines max # of palette enteries, not the precision
    - grayscale: pixel is represented by a single sample with 0 being black and the largest value being white
    - truecolor: three samples (RGB). Bit depth specifies size of each sample, not the total pixel size.
    - Grayscale and trucolor can also incldue an alpha sample
    - Pixels smaller than a byte never cross byte boundaries
    - bits arent wasted between pixels
    - PNG permits multiple sample pixels with 8- and 16-bit samples so multiple samples of a single pixel are never packed into one byte. 16 bit samples are in network byte order (MSB first)
    - Scanlines begin on byte boundaries meaning the end of the scanline may waste a few bits
    - a 'filter type' byte is added to the beginning of every scanline
- Alpha Channel
    - transparency information on a per-pixel basis for grayscale and trucolor
    - 0: full transperency and 2^bitdepth is fully opaque
    - only for images with 8/16 bits/sample but not for those with <8/sample
    - immediately follows the grayscale or RGB samples of the pixel
    - Filtering
        - TODO/ignore
    - Interlaced Data order
        - TODO/ignore
    - Gamma correction
        - TODO/ignore
    - Text strings
        - TODO/ignore

## PNG Data Layout
```
[8 bytes PNG file signature][1 IHDR Chunk][>=1 IDAT Chunks][1 IEND Chunk]
```
 
## Plan to Support
- Grayscale

## Plan to Ignore
- Multiple Sampling
- Indexed color
- Alpha channel
- Filter types
- Gamma correction
- Text stringis

## Progress
- fix logging in png_logger.h
- On section 4.1 now, critical chunks
