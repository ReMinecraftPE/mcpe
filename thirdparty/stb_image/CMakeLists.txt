cmake_minimum_required(VERSION 3.16.0)
project(stb_image)

# Build
add_library(stb_image STATIC src/stb_image_impl.c)
target_include_directories(stb_image PUBLIC include)
add_library(stb_vorbis STATIC include/stb_vorbis.c)
target_include_directories(stb_vorbis PUBLIC include)
