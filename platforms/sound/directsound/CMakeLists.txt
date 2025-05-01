cmake_minimum_required(VERSION 3.16.0)
project(reminecraftpe-directsound)

# Build
add_library(reminecraftpe-sound STATIC
    SoundSystemDS.cpp
)

# Libraries
target_link_libraries(reminecraftpe-sound
    reminecraftpe-core
    dsound
    dxguid
)

# Headers
target_include_directories(reminecraftpe-sound PUBLIC .)
