cmake_minimum_required(VERSION 3.16.0)
project(reminecraftpe-sdl-desktop)

# Build
add_library(reminecraftpe-sdl-platform STATIC
    AppPlatform_sdl.cpp
)

# Core
target_link_libraries(reminecraftpe-sdl-platform reminecraftpe-core)
# SDL Base
target_link_libraries(reminecraftpe-sdl-platform reminecraftpe-sdl-base)

# Headers
target_include_directories(reminecraftpe-sdl-platform PUBLIC .)

# LibPNG
find_package(PNG REQUIRED)
target_link_libraries(reminecraftpe-sdl-platform PNG::PNG)

# Assets
file(CREATE_LINK "${CMAKE_SOURCE_DIR}/../../game/assets" "${CMAKE_BINARY_DIR}/assets" SYMBOLIC)
