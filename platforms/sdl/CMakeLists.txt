cmake_minimum_required(VERSION 3.16.0)
project(reminecraftpe)

# SDL Build
add_compile_definitions(USE_SDL HANDLE_CHARS_SEPARATELY)

# WASM
if(EMSCRIPTEN)
    function(add_compile_and_link_options)
        add_compile_options(${ARGV})
        add_link_options(${ARGV})
    endfunction()
    set(CMAKE_EXECUTABLE_SUFFIX ".js")
endif()

# Clang
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-Wno-inconsistent-missing-override -Wno-enum-compare-switch -Wno-register)
endif()

# Threads
if(EMSCRIPTEN)
    add_compile_and_link_options(-pthread)
else()
    find_package(Threads)
    link_libraries(Threads::Threads)
endif()

# Build
add_executable(reminecraftpe
    main.cpp
    AppPlatform_sdl.cpp
    SoundSystemAL.cpp
)

# Core
add_subdirectory(../../source source)
target_link_libraries(reminecraftpe reminecraftpe-core)

# LibPNG
if(NOT EMSCRIPTEN)
    find_package(PNG REQUIRED)
    target_link_libraries(reminecraftpe PNG::PNG)
endif()

# SDL
if(TARGET SDL2::SDL2main)
    target_link_libraries(reminecraftpe SDL2::SDL2main)
endif()

# WASM
if(EMSCRIPTEN)
    target_link_options(reminecraftpe PRIVATE -Wno-pthreads-mem-growth)
    target_link_options(reminecraftpe PRIVATE -sALLOW_MEMORY_GROWTH=1)
    # Export Resize Function
    target_link_options(reminecraftpe PRIVATE -sEXPORTED_FUNCTIONS=_main,_resize_from_js -sEXPORTED_RUNTIME_METHODS=ccall)
endif()

# Assets
if(EMSCRIPTEN)
    target_link_options(reminecraftpe PRIVATE --use-preload-plugins --preload-file "${CMAKE_CURRENT_SOURCE_DIR}/assets@/assets")
elseif(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/assets")
    file(CREATE_LINK "${CMAKE_CURRENT_SOURCE_DIR}/assets" "${CMAKE_CURRENT_BINARY_DIR}/assets" SYMBOLIC)
endif()
