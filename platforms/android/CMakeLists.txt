cmake_minimum_required(VERSION 3.16.0)
project(reminecraftpe-android)

# Native Android Build
target_compile_definitions(reminecraftpe-core PUBLIC USE_NATIVE_ANDROID)

# Build
add_library(reminecraftpe SHARED
    android_native_app_glue.c
    AppPlatform_android.cpp
    main.cpp
)

# Core
target_link_libraries(reminecraftpe reminecraftpe-core)

# OpenGL
target_link_libraries(reminecraftpe-core PUBLIC EGL GLESv1_CM)

# stb_image
target_link_libraries(reminecraftpe stb_image)

# Extra Dependencies
target_link_libraries(reminecraftpe android)

# Check for the presence of some optional asset based features.
if(EXISTS "${MC_ROOT}/game/assets/gui/background/panorama_0.png")
    target_compile_definitions(reminecraftpe PUBLIC FEATURE_MENU_BACKGROUND)
endif()
if(EXISTS "${MC_ROOT}/game/assets/environment/clouds.png")
    target_compile_definitions(reminecraftpe PUBLIC FEATURE_CLOUDS)
endif()
if(EXISTS "${MC_ROOT}/game/assets/misc/grasscolor.png")
    target_compile_definitions(reminecraftpe PUBLIC FEATURE_GRASS_COLOR)
endif()
if(EXISTS "${MC_ROOT}/game/assets/misc/foliagecolor.png")
    target_compile_definitions(reminecraftpe PUBLIC FEATURE_FOLIAGE_COLOR)
endif()
