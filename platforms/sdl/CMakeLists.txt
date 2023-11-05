cmake_minimum_required(VERSION 3.16.0)
project(reminecraftpe-sdl)

# SDL Build
add_compile_definitions(USE_SDL USE_OPENAL HANDLE_CHARS_SEPARATELY)
set(USE_SDL TRUE)

# WASM
if(EMSCRIPTEN)
    function(add_compile_and_link_options)
        add_compile_options(${ARGV})
        add_link_options(${ARGV})
    endfunction()
    set(CMAKE_EXECUTABLE_SUFFIX ".js")
    add_link_options("$<$<CONFIG:DEBUG>:-gsource-map>")
endif()

# Clang
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-Wno-inconsistent-missing-override -Wno-enum-compare-switch -Wno-register)
endif()

# Network library
if(HAIKU)
    link_libraries(network)
endif()

# Threads
if(EMSCRIPTEN)
    add_compile_and_link_options(-pthread)
else()
    find_package(Threads)
    link_libraries(Threads::Threads)
endif()

# Build
set(SOURCES
    main.cpp
    base/AppPlatform_sdl_base.cpp
)
if(EMSCRIPTEN)
    list(APPEND SOURCES emscripten/AppPlatform_sdl.cpp)
else()
    list(APPEND SOURCES desktop/AppPlatform_sdl.cpp)
endif()
if(ANDROID)
    add_library(reminecraftpe SHARED ${SOURCES})
    set_target_properties(reminecraftpe PROPERTIES OUTPUT_NAME main)
else()
    add_executable(reminecraftpe ${SOURCES})
endif()

# Core
add_subdirectory(../../source source)
target_link_libraries(reminecraftpe reminecraftpe-core)

# OpenAL
add_subdirectory(../openal openal)
target_link_libraries(reminecraftpe reminecraftpe-openal)

# stb_image (If Needed)
if(NOT EMSCRIPTEN)
    add_subdirectory(../../thirdparty/stb_image stb_image)
    target_link_libraries(reminecraftpe stb_image)
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
    target_link_options(reminecraftpe PRIVATE --use-preload-plugins --preload-file "${CMAKE_CURRENT_SOURCE_DIR}/../../game@/")
elseif(NOT ANDROID)
    file(CREATE_LINK "${CMAKE_CURRENT_SOURCE_DIR}/../../game/assets" "${CMAKE_CURRENT_BINARY_DIR}/assets" SYMBOLIC)
endif()
