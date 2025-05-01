cmake_minimum_required(VERSION 3.16.0)
project(reminecraftpe-opensl)

# Build
add_library(reminecraftpe-sound STATIC
	SoundSystemSL.cpp
)

# Core
target_link_libraries(reminecraftpe-sound PUBLIC reminecraftpe-core)

# OpenSL
target_link_libraries(reminecraftpe-sound PUBLIC OpenSLES)

# Headers
target_include_directories(reminecraftpe-sound PUBLIC .)
