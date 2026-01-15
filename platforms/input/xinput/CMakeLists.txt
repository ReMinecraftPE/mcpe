cmake_minimum_required(VERSION 3.16.0)
project(reminecraftpe-input-xinput)

# Build
add_library(reminecraftpe-input STATIC
	GameControllerHandler_xinput.cpp
)

target_link_libraries(reminecraftpe-input PUBLIC reminecraftpe-core)

# Headers
target_include_directories(reminecraftpe-input PUBLIC .)
