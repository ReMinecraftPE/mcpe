cmake_minimum_required(VERSION 3.16.0)
project(reminecraftpe-audio-xaudio2)

# Build
add_library(reminecraftpe-audio STATIC
    SoundSystemXA2.cpp
)

# Libraries
target_link_libraries(reminecraftpe-audio
    reminecraftpe-core
    xaudio2
)

# Headers
target_include_directories(reminecraftpe-audio PUBLIC .)
