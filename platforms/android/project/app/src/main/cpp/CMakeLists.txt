
# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.16.0)

# Declares and names the project.

project("reminecraftpe")

# Add the ANDROID flag. Used by our core
set(ANDROID)

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

# Add the core as part of the library.
add_subdirectory(${MC_ROOT}/source source)
target_link_libraries(reminecraftpe reminecraftpe-core)

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

# find_library(nameofpathvariable, nameoftheNDKlibrary)
find_library(log-lib log)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
                       reminecraftpe

                       # Links the target library to log, android, GLESv1_CM and zlib
                       ${log-lib} android EGL GLESv1_CM z )
