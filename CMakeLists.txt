cmake_minimum_required(VERSION 3.14)
project(png_encoder_decoder)

# turn on all gcc compiler warnings
if(MSVC)
    add_compile_option(-W4 -WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
    # add_compiler_options(-Ofast)
    # add_compiler_options(-O3)
endif()

# add_subdirectory(src)
# add_subdirectory(tests)
set(CMAKE_CXX_FLAGS "-std=c++17")
include_directories(./include)

add_library(
    png_decoder
    STATIC
    ./include/png_decoder.h
    ./src/png_decoder.c
)

add_executable(main.exe ./src/main.c)
target_link_libraries(main.exe PRIVATE png_decoder)
# add_subdirectory(tests)

################################
#           GoogleTest         #
################################
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/5197b1a8e6a1ef9f214f4aa537b0be17cbf91946.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
    test_png_decoder
    tests/test_png_decoder.cpp
)
target_link_libraries(
    test_png_decoder
    GTest::gtest_main
    png_decoder
)

include(GoogleTest)
gtest_discover_tests(test_png_decoder)