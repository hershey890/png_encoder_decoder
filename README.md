# PNG Encoder and Decoder
Personal education focused project aimed at writing a PNG encoders and decoder in C.
- Second version to be done in C by incrementally substituting Python modules with C modules.

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

## Supported Features
* Nothing so far

## Unsupported Features
* PNG Decoder
* PNG Encoder

## Resources
- [PNG RFC](http://www.libpng.org/pub/png/spec/1.2/PNG-Contents.html)
- [PNG Spec](https://www.w3.org/TR/png/)
- [PNG file chunk inspector](https://www.nayuki.io/page/png-file-chunk-inspector)