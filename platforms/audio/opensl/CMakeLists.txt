cmake_minimum_required(VERSION 3.16.0)
project(nbcraft-audio-opensl)

# Build
add_library(nbcraft-audio STATIC
	SoundSystemOSL.cpp
)

# Core
target_link_libraries(nbcraft-audio PUBLIC nbcraft-core)

# OpenSL
target_link_libraries(nbcraft-audio PUBLIC OpenSLES)

# Headers
target_include_directories(nbcraft-audio PUBLIC .)
