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

set(CMAKE_CXX_FLAGS "-std=c++20")
set(CMAKE_BUILD_TYPE Debug)
include_directories(include)

add_library(
    png_decoder
    STATIC
    src/png_decoder.c
    src/png_validate.c
)

add_executable(main.exe ./src/main.c)
target_link_libraries(main.exe PRIVATE png_decoder)

################################
#           GoogleTest         #
################################
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

# set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_definitions(-DGOOGLE_TEST)
add_executable(
    test_png_decoder
    tests/test_png_decoder.cpp
)
target_link_libraries(
    test_png_decoder
    GTest::gtest_main
    png_decoder
)
target_include_directories(
    test_png_decoder
    PRIVATE
    ${PROJECT_SOURCE_DIR}/src
)

# add_test(
    # NAME test_png_decoder
    # COMMAND test_png_decoder
# )

include(GoogleTest)
gtest_discover_tests(test_png_decoder)