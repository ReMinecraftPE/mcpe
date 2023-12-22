cmake_minimum_required(VERSION 3.16.0)
project(reminecraftpe-android)

# Project Root
set(MC_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../..")

# Native Android Build
add_compile_definitions(USE_NATIVE_ANDROID)
set(USE_NATIVE_ANDROID TRUE)

# Build
add_library(reminecraftpe SHARED
    "${MC_ROOT}/platforms/android/android_native_app_glue.c"
    "${MC_ROOT}/platforms/android/AppPlatform_android.cpp"
    "${MC_ROOT}/platforms/android/SoundSystemSL.cpp"
    "${MC_ROOT}/platforms/android/main.cpp"
)

# Core
add_subdirectory("${MC_ROOT}/source" source)
target_link_libraries(reminecraftpe reminecraftpe-core)

# stb_image
add_subdirectory("${MC_ROOT}/thirdparty/stb_image" stb_image)
target_link_libraries(reminecraftpe stb_image)

# Extra Dependencies
target_link_libraries(reminecraftpe android OpenSLES)

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
