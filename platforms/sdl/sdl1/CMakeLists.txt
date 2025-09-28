cmake_minimum_required(VERSION 3.16.0)
project(reminecraftpe-sdl1)

# SDL Build
target_compile_definitions(reminecraftpe-core
    PUBLIC USE_SDL
    PUBLIC USE_SDL1
    PUBLIC HANDLE_CHARS_SEPARATELY
)

# Build
set(SOURCES
    main.cpp
    base/AppPlatform_sdl1.cpp
    ../base/AppPlatform_sdl.cpp
)
list(APPEND SOURCES desktop/AppPlatform_sdl1_desktop.cpp)
add_executable(reminecraftpe ${SOURCES})

# Core
target_link_libraries(reminecraftpe reminecraftpe-core)

# stb_image (If Needed)
target_link_libraries(reminecraftpe stb_image)

# SDL (From system)
find_package(SDL REQUIRED)
target_include_directories(reminecraftpe-core PUBLIC "${SDL_INCLUDE_DIR}")
target_link_libraries(reminecraftpe-core PUBLIC "${SDL_LIBRARY}")
if(SDLMAIN_LIBRARY)
    target_link_libraries(reminecraftpe "${SDLMAIN_LIBRARY}")
endif()

# OpenGL
find_package(OpenGL REQUIRED)
target_link_libraries(reminecraftpe-core PUBLIC OpenGL::GL)
