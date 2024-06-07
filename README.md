# PNG Encoder and Decoder
Personal project aimed at writing PNG encoders and decoders in Python and C.
- Initial implementation to be done in Python
- Second version to be done in C by incrementally substituting Python modules with C moduels

## Building and Running
For CMake users:
```
$ cmake -S . -B build
$ cmake --build build
$ (cd build && main.exe)
```
The executables will now available in `./build/{sub project name}/`. No manual file download is needed for GoogleTest is needed, the CMake script pulls everything in.

If you do not use Cmake:
```
$ gcc -Iinclude -o main.exe src/png_decoder.c src/main.c
```

## Testing
(Based on [this GoogleTest guide](https://google.github.io/googletest/quickstart-cmake.html)). To build and run the tests, use the following:  
```
$ cmake -S . -B build
$ cmake --build build
$ (cd build && ctest)
```

## Supported Features

## Unsupported Features

## Resources
- [PNG Specification](http://www.libpng.org/pub/png/spec/1.2/PNG-Contents.html)
