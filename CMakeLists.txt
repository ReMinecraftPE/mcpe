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

# Detect Windows
include(CheckSymbolExists)
check_symbol_exists("_WIN32" "" MCPE_WIN32)

# Clang
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-Wno-inconsistent-missing-override -Wno-enum-compare-switch -Wno-register)
endif()

# Windows Linking
if(MCPE_WIN32)
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

# stb_image And Other Libraries
add_subdirectory(thirdparty/stb_image EXCLUDE_FROM_ALL)

# Load Common Code
add_subdirectory(source)

# Load Platform-Specific Code
add_subdirectory(platforms)

# Assets
if(EMSCRIPTEN)
    target_link_options(reminecraftpe PRIVATE --use-preload-plugins --preload-file "${CMAKE_CURRENT_SOURCE_DIR}/game@/")
else()
    file(CREATE_LINK "${CMAKE_CURRENT_SOURCE_DIR}/game/assets" "${CMAKE_CURRENT_BINARY_DIR}/assets" SYMBOLIC)
endif()

#VBO Emulation
if(USE_GL_VBO_EMULATION)
    add_compile_options(-DUSE_GL_VBO_EMULATION)
endif()