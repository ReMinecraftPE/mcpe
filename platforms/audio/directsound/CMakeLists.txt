cmake_minimum_required(VERSION 3.16.0)
project(reminecraftpe-audio-directsound)

# Build
add_library(reminecraftpe-audio STATIC
    SoundSystemDS.cpp
    SoundStreamDS.cpp
)

# Libraries
target_link_libraries(reminecraftpe-audio
    reminecraftpe-core
    dsound
    dxguid
)

# Headers
target_include_directories(reminecraftpe-audio PUBLIC .)
