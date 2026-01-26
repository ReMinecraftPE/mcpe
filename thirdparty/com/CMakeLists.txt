cmake_minimum_required(VERSION 3.16.0)
project(com)

# Build
add_library(com STATIC
	ComInterface.cpp
)
target_include_directories(com PUBLIC .)
