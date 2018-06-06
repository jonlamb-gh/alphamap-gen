cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

project(alphamap-gen VERSION 0.1 LANGUAGES C)

# src/lodepng.c is a symlink to vendor/lodepng/lodepng.cpp
add_executable(
    alphamap-gen
    src/lodepng.c
    vendor/libattopng/libattopng.c
    src/util.c
    src/main.c)

set_property(
    TARGET
    alphamap-gen
    PROPERTY
    C_STANDARD
    99)

target_compile_options(
    alphamap-gen
    PRIVATE
    -Wall
    -Wextra
    -Wno-unused-parameter
    -Wwrite-strings
    -Wstrict-prototypes
    -Wmissing-include-dirs
    -Werror)

target_compile_definitions(
    alphamap-gen
    PRIVATE
    LODEPNG_NO_COMPILE_CPP)

target_include_directories(
    alphamap-gen
    PRIVATE
    include
    vendor/libattopng
    vendor/lodepng)

target_link_libraries(
    alphamap-gen
    -static-libgcc
    -static
    -lpopt)

install(
    TARGETS
    alphamap-gen
    DESTINATION
    /usr/local/bin)

