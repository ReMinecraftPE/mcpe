cmake_minimum_required(VERSION 3.16.0)
project(nbcraft-audio-directsound)

# Build
add_library(nbcraft-audio STATIC
    SoundSystemDS.cpp
    SoundStreamDS.cpp
)

# Libraries
target_link_libraries(nbcraft-audio
    nbcraft-core
    dsound
    dxguid
)

# Headers
target_include_directories(nbcraft-audio PUBLIC .)
