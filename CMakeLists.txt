cmake_minimum_required(VERSION 3.16.0)
project(reminecraftpe)

# Store Output In Top-Level Build Directory
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

# Load Common Code
add_subdirectory(source)

# Load Platform
set(DEFAULT_PLATFORM "sdl")
if(WIN32)
    set(DEFAULT_PLATFORM "windows")
endif()
set(REMCPE_PLATFORM "${DEFAULT_PLATFORM}" CACHE STRING "ReMCPE Platform (Check <root>/platforms)")
add_subdirectory("platforms/${REMCPE_PLATFORM}")

# Assets
file(CREATE_LINK "${CMAKE_CURRENT_SOURCE_DIR}/game/assets" "${CMAKE_CURRENT_BINARY_DIR}/assets" SYMBOLIC)
