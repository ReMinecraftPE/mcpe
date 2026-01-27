cmake_minimum_required(VERSION 3.16.0)
project(reminecraftpe-audio-dummy)

# Build
add_library(reminecraftpe-audio STATIC
	SoundSystemNull.cpp
)

target_link_libraries(reminecraftpe-audio PUBLIC reminecraftpe-core)

# Headers
target_include_directories(reminecraftpe-audio PUBLIC .)
