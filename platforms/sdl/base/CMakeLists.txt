cmake_minimum_required(VERSION 3.16.0)
project(reminecraftpe-sdl-base)

# Build
add_library(reminecraftpe-sdl-base STATIC
	AppPlatform_sdl_base.cpp
)

# Core
target_link_libraries(reminecraftpe-sdl-base reminecraftpe-core)

# Headers
target_include_directories(reminecraftpe-sdl-base PUBLIC .)

# OpenAL
add_subdirectory(../../openal openal)
target_link_libraries(reminecraftpe-sdl-base reminecraftpe-openal)
