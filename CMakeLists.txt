cmake_minimum_required(VERSION 3.16.0)
project(reminecraftpe)

# Store Output In Top-Level Build Directory
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

# WASM
if(EMSCRIPTEN)
    function(add_compile_and_link_options)
        add_compile_options(${ARGV})
        add_link_options(${ARGV})
    endfunction()
    set(CMAKE_EXECUTABLE_SUFFIX ".js")
    add_link_options("$<$<CONFIG:DEBUG>:-gsource-map>")
    add_link_options(-Wno-pthreads-mem-growth -sALLOW_MEMORY_GROWTH=1)
endif()

# Clang
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-Wno-inconsistent-missing-override -Wno-enum-compare-switch -Wno-register)
endif()

# Windows Linking
if(WIN32)
    add_link_options(
        -static-libgcc
        -static-libstdc++
    )
endif()

# HaikuOS Network Library
if(HAIKU)
    link_libraries(network)
endif()

# Threading
if(EMSCRIPTEN)
    add_compile_and_link_options(-pthread)
else()
    find_package(Threads)
    link_libraries(Threads::Threads)
endif()

# Android Logging
if(ANDROID)
    link_libraries(log)
endif()

# Load Common Code
add_subdirectory(source)

# Load Platform
set(DEFAULT_PLATFORM "sdl")
if(WIN32)
    set(DEFAULT_PLATFORM "windows")
endif()
set(REMCPE_PLATFORM "${DEFAULT_PLATFORM}" CACHE STRING "ReMCPE Platform (Check <root>/platforms)")
add_subdirectory("platforms/${REMCPE_PLATFORM}")

# Load Sound Platform
set(DEFAULT_SOUND_PLATFORM "openal")
if(WIN32)
    set(DEFAULT_SOUND_PLATFORM "directsound")
endif()
set(REMCPE_SOUND_PLATFORM "${DEFAULT_SOUND_PLATFORM}" CACHE STRING "ReMCPE Sound Platform (Check <root>/platforms/sound)")
add_subdirectory("platforms/sound/${REMCPE_SOUND_PLATFORM}")
target_link_libraries(reminecraftpe reminecraftpe-sound)

# Assets
if(EMSCRIPTEN)
    target_link_options(reminecraftpe PRIVATE --use-preload-plugins --preload-file "${CMAKE_CURRENT_SOURCE_DIR}/game@/")
else()
    file(CREATE_LINK "${CMAKE_CURRENT_SOURCE_DIR}/game/assets" "${CMAKE_CURRENT_BINARY_DIR}/assets" SYMBOLIC)
endif()