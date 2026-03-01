cmake_minimum_required(VERSION 3.16.0)
project(nbcraft-android)

# Native Android Build
target_compile_definitions(nbcraft-core PUBLIC USE_NATIVE_ANDROID)

# Build
add_library(nbcraft SHARED
    android_native_app_glue.c
    AppPlatform_android.cpp
    main.cpp
)

# Core
target_link_libraries(nbcraft nbcraft-core)

# OpenGL
target_link_libraries(nbcraft-core PUBLIC EGL GLESv1_CM)

# stb_image
target_link_libraries(nbcraft stb_image)

# Extra Dependencies
target_link_libraries(nbcraft android)

# Check for the presence of some optional asset based features.
if(EXISTS "${MC_ROOT}/game/assets/gui/background/panorama_0.png")
    target_compile_definitions(nbcraft PUBLIC FEATURE_MENU_BACKGROUND)
endif()
if(EXISTS "${MC_ROOT}/game/assets/environment/clouds.png")
    target_compile_definitions(nbcraft PUBLIC FEATURE_CLOUDS)
endif()
if(EXISTS "${MC_ROOT}/game/assets/misc/grasscolor.png")
    target_compile_definitions(nbcraft PUBLIC FEATURE_GRASS_COLOR)
endif()
if(EXISTS "${MC_ROOT}/game/assets/misc/foliagecolor.png")
    target_compile_definitions(nbcraft PUBLIC FEATURE_FOLIAGE_COLOR)
endif()
