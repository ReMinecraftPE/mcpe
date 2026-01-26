cmake_minimum_required(VERSION 3.16.0)
project(reminecraftpe-sdl2)

# SDL Build
target_compile_definitions(reminecraftpe-core
    PUBLIC USE_SDL
    PUBLIC HANDLE_CHARS_SEPARATELY
)

# Build
set(SOURCES
    main.cpp
    base/AppPlatform_sdl2.cpp
    ../base/AppPlatform_sdl.cpp
)
if(EMSCRIPTEN)
    list(APPEND SOURCES emscripten/AppPlatform_sdl2_emscripten.cpp)
else()
    list(APPEND SOURCES desktop/AppPlatform_sdl2_desktop.cpp)
endif()
if(ANDROID)
    add_library(reminecraftpe SHARED ${SOURCES})
    set_target_properties(reminecraftpe PROPERTIES OUTPUT_NAME main)
else()
    add_executable(reminecraftpe ${SOURCES})
endif()

# Core
target_link_libraries(reminecraftpe reminecraftpe-core)

# stb_image
target_link_libraries(reminecraftpe stb_image)

# SDL
add_library(SDL INTERFACE)
if(ANDROID OR REMCPE_WIN32)
    # Use Vendored SDL2 (Only For Android)
    add_subdirectory(../../../thirdparty/SDL2/src SDL EXCLUDE_FROM_ALL)
    set_target_properties(SDL2 PROPERTIES CXX_STANDARD 11)
    target_link_libraries(SDL INTERFACE SDL2::SDL2)
    target_compile_options(SDL2 PRIVATE -w)
elseif(EMSCRIPTEN)
    # Use Emscripten's SDL2
    set(SDL_FLAG -sUSE_SDL=2)
    target_compile_options(SDL INTERFACE "${SDL_FLAG}")
    target_link_options(SDL INTERFACE "${SDL_FLAG}")
else()
    # Use System SDL2
    find_package(SDL2 REQUIRED)
    target_link_libraries(SDL INTERFACE SDL2::SDL2)
endif()
target_link_libraries(reminecraftpe-core PUBLIC SDL)
if(TARGET SDL2::SDL2main)
    target_link_libraries(reminecraftpe SDL2::SDL2main)
endif()

if(REMCPE_GFX_API STREQUAL "OGL")
    # OpenGL
    if(ANDROID)
        find_library(GLES_LIB GLESv1_CM REQUIRED)
        target_link_libraries(reminecraftpe-core PUBLIC "${GLES_LIB}")
        target_compile_definitions(reminecraftpe-core PUBLIC USE_GLES)
    elseif(NINTENDO_SWITCH)
        target_link_libraries(reminecraftpe-core PUBLIC glapi GLESv1_CM drm_nouveau)
        target_compile_definitions(reminecraftpe-core PUBLIC USE_GLES)
    else()
        find_package(OpenGL REQUIRED)
        target_link_libraries(reminecraftpe-core PUBLIC OpenGL::GL)
        if(EMSCRIPTEN)
            # Use WebGL 2
            target_link_options(reminecraftpe-core PUBLIC
                -sMIN_WEBGL_VERSION=2 -sMAX_WEBGL_VERSION=2
                -sLEGACY_GL_EMULATION -sGL_FFP_ONLY
            )
        endif()
    endif()
endif()

if(EMSCRIPTEN) # WASM
    # Export Resize Function
    target_link_options(reminecraftpe PRIVATE -sEXPORTED_FUNCTIONS=_main,_resize_from_js -sEXPORTED_RUNTIME_METHODS=ccall)
endif()

if(NINTENDO_SWITCH)
    nx_generate_nacp(reminecraftpe.nacp
        NAME "ReMinecraftPE"
        AUTHOR "ReMinecraftPE Team"
        VERSION "v0.2.0 alpha"
    )

    nx_create_nro(reminecraftpe
        NACP reminecraftpe.nacp
        ICON "../../../game/assets/icon.jpg"
    )
endif()
