cmake_minimum_required(VERSION 3.14)
project(main)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(
    png_decoder
    STATIC
    ../include/png_decoder.h
    png_decoder.c
)

add_executable(main.exe main.c)
target_link_libraries(main.exe PRIVATE png_decoder)