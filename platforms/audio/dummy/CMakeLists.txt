cmake_minimum_required(VERSION 3.16.0)
project(nbcraft-audio-dummy)

# Build
add_library(nbcraft-audio STATIC
	SoundSystemNull.cpp
)

target_link_libraries(nbcraft-audio PUBLIC nbcraft-core)

# Headers
target_include_directories(nbcraft-audio PUBLIC .)
