cmake_minimum_required(VERSION 3.16.0)
project(reminecraftpe)

# Store Output In Top-Level Build Directory
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_EXTENSIONS OFF)

# Warnings
add_compile_options(-Wall -Wignored-qualifiers)
add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-Wc++11-extensions>)
if(WERROR)
    add_compile_options(-Werror)
endif()
if(APPLE)
    add_compile_options(-Wno-deprecated-declarations)
endif()

# WASM
if(EMSCRIPTEN)
    function(add_compile_and_link_options)
        add_compile_options(${ARGV})
        add_link_options(${ARGV})
    endfunction()
    set(CMAKE_EXECUTABLE_SUFFIX ".js")
    add_link_options("$<$<CONFIG:DEBUG>:-gsource-map>")
    add_link_options(-Wno-pthreads-mem-growth -sALLOW_MEMORY_GROWTH=1)
	add_compile_definitions(MC_NO_NETWORKING)
endif()

# Detect Windows
include(CheckSymbolExists)
check_symbol_exists("_WIN32" "" REMCPE_WIN32)

# Windows Linking
if(REMCPE_WIN32)
    add_link_options(
        -static-libgcc
        -static-libstdc++
    )
    # Modern MinGW doesn't support C++98 and will error if you try to use it
    set(CMAKE_CXX_STANDARD 11)
else()
    set(CMAKE_CXX_STANDARD 98)
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

# Disable thread local storage (needed for C++98 & iOS)
add_compile_definitions(RAPIDJSON_NO_THREAD_LOCAL STBI_NO_THREAD_LOCALS)

if(NOT DEFINED REMCPE_GFX_API)
	message(STATUS "No GFX API specified, defaulting to OGL...")
	set(REMCPE_GFX_API "OGL" CACHE STRING "HAL GFX API")
endif()

message(STATUS "Selected GFX API: ${REMCPE_GFX_API}")

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

# VBO Emulation
if(USE_GL_VBO_EMULATION)
    add_compile_definitions(USE_GL_VBO_EMULATION)
endif()

# Disable warnings on third party libraries
target_compile_options(stb_vorbis PRIVATE -w)
target_compile_options(stb_image PRIVATE -w)
target_compile_options(raknet PRIVATE -w)
