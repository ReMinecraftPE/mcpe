
# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.16.0)

# Declares and names the project.

project("reminecraftpe")

# The root of the project
SET(MC_ROOT ../../../../../../..)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library( # Sets the name of the library.
             reminecraftpe

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             ${MC_ROOT}/platforms/android/android_native_app_glue.c
             ${MC_ROOT}/platforms/android/AppPlatform_android.cpp
             ${MC_ROOT}/platforms/android/main.cpp 
             ${MC_ROOT}/thirdparty/stb_image_impl.c)

# Check for the presence of some optional asset based features.
if(NOT EXISTS "${MC_ROOT}/game/assets/gui/background/panorama_0.png")
	target_compile_definitions(reminecraftpe PUBLIC FEATURE_MENU_BACKGROUND)
endif()

if(NOT EXISTS "${MC_ROOT}/game/assets/environment/clouds.png")
	target_compile_definitions(reminecraftpe PUBLIC FEATURE_CLOUDS)
endif()

if(NOT EXISTS "${MC_ROOT}/game/assets/misc/grasscolor.png")
	target_compile_definitions(reminecraftpe PUBLIC FEATURE_GRASS_COLOR)
endif()

if(NOT EXISTS "${MC_ROOT}/game/assets/misc/foliagecolor.png")
	target_compile_definitions(reminecraftpe PUBLIC FEATURE_FOLIAGE_COLOR)
endif()

# Add the core as part of the library.
add_subdirectory(${MC_ROOT}/source source)
target_link_libraries(reminecraftpe reminecraftpe-core)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
                       reminecraftpe android )
