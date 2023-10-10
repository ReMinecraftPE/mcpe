cmake_minimum_required(VERSION 3.16.0)
project(reminecraftpe-sdl-emscripten)

# Build
add_library(reminecraftpe-sdl-platform STATIC
    AppPlatform_sdl_emscripten.cpp
)

# Core
target_link_libraries(reminecraftpe-sdl-platform reminecraftpe-core)
# SDL Base
target_link_libraries(reminecraftpe-sdl-platform reminecraftpe-sdl-base)

# Headers
target_include_directories(reminecraftpe-sdl-platform PUBLIC .)

# WASM
target_link_options(reminecraftpe-sdl-platform PUBLIC -Wno-pthreads-mem-growth)
target_link_options(reminecraftpe-sdl-platform PUBLIC -sALLOW_MEMORY_GROWTH=1)
# Export Resize Function
target_link_options(reminecraftpe-sdl-platform PUBLIC -sEXPORTED_FUNCTIONS=_main,_resize_from_js -sEXPORTED_RUNTIME_METHODS=ccall)

# Assets
target_link_options(reminecraftpe-sdl-platform PUBLIC --use-preload-plugins --preload-file "${CMAKE_SOURCE_DIR}/../../game/assets@/assets")
