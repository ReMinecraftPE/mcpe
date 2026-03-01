cmake_minimum_required(VERSION 3.16.0)
project(nbcraft-audio-xaudio2)

# Build
add_library(nbcraft-audio STATIC
    SoundSystemXA2.cpp
)

# Libraries
target_link_libraries(nbcraft-audio
    nbcraft-core
    xaudio2
)

# Headers
target_include_directories(nbcraft-audio PUBLIC .)
