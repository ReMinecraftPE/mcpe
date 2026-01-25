cmake_minimum_required(VERSION 3.16.0)
project(reminecraftpe-audio-opensl)

# Build
add_library(reminecraftpe-audio STATIC
	SoundSystemOSL.cpp
)

# Core
target_link_libraries(reminecraftpe-audio PUBLIC reminecraftpe-core)

# OpenSL
target_link_libraries(reminecraftpe-audio PUBLIC OpenSLES)

# Headers
target_include_directories(reminecraftpe-audio PUBLIC .)
