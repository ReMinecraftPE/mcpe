cmake_minimum_required(VERSION 3.16.0)
project(reminecraftpe-sdl1)

# SDL Build
target_compile_definitions(reminecraftpe-core
    PUBLIC USE_SDL
    PUBLIC HANDLE_CHARS_SEPARATELY
)

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
target_link_libraries(reminecraftpe reminecraftpe-core)

# stb_image (If Needed)
if(NOT EMSCRIPTEN)
    target_link_libraries(reminecraftpe stb_image)
endif()

# SDL
add_library(SDL INTERFACE)

# Use System SDL
find_package(SDL REQUIRED)
include_directories(${SDL_INCLUDE_DIR})
target_link_libraries(SDL INTERFACE ${SDL_LIBRARY})

target_link_libraries(reminecraftpe-core PUBLIC SDL)
if(SDLMAIN_LIBRARY)
    target_link_libraries(reminecraftpe ${SDLMAIN_LIBRARY})
endif()

# OpenGL
find_package(OpenGL REQUIRED)
target_link_libraries(reminecraftpe-core PUBLIC OpenGL::GL)