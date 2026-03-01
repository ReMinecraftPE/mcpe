cmake_minimum_required(VERSION 3.16.0)
project(nbcraft-input-xinput)

# Build
add_library(nbcraft-input STATIC
	GameControllerHandler_xinput.cpp
)

target_link_libraries(nbcraft-input
	PUBLIC xinput
)

target_link_libraries(nbcraft-input PUBLIC nbcraft-core)

# Headers
target_include_directories(nbcraft-input PUBLIC .)
