cmake_minimum_required(VERSION 3.16.0)
project(libxenon_utils)

# Build
add_library(libxenon_utils STATIC
    pthread.c
)
target_include_directories(libxenon_utils PUBLIC .)
