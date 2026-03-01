cmake_minimum_required(VERSION 3.16.0)
project(nbcraft-android)

# Project Root
set(NBC_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../..")

# Native Android Build
add_compile_definitions(USE_NATIVE_ANDROID)
set(USE_NATIVE_ANDROID TRUE)

# Build
add_library(nbcraft SHARED
    "${NBC_ROOT}/platforms/android/android_native_app_glue.c"
    "${NBC_ROOT}/platforms/android/AppPlatform_android.cpp"
    "${NBC_ROOT}/platforms/android/main.cpp"
)

# Core
add_subdirectory("${NBC_ROOT}/source" source)
target_link_libraries(nbcraft nbcraft-core)

# Audio
add_subdirectory("${NBC_ROOT}/platforms/audio/opensl" platforms_audio_opensl)
target_link_libraries(nbcraft nbcraft-audio)

# OpenGL
target_link_libraries(nbcraft-core PUBLIC android EGL GLESv1_CM log)

# stb_image
add_subdirectory("${NBC_ROOT}/thirdparty/stb_image" stb_image)
target_link_libraries(nbcraft stb_image)

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
