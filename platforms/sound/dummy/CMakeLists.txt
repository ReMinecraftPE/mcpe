cmake_minimum_required(VERSION 3.16.0)
project(reminecraftpe-dummy)

# Build
add_library(reminecraftpe-sound STATIC
	SoundSystem.cpp
	SoundStream.cpp
)

target_link_libraries(reminecraftpe-sound PUBLIC reminecraftpe-core)

# Headers
target_include_directories(reminecraftpe-sound PUBLIC .)
