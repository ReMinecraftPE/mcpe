cmake_minimum_required(VERSION 3.16.0)
project(reminecraftpe-openal)

# Build
add_library(reminecraftpe-sound STATIC
	SoundSystemAL.cpp
	SoundStreamAL.cpp
)

# Core
target_link_libraries(reminecraftpe-sound PUBLIC reminecraftpe-core)

# OpenAL
if(ANDROID)
    # Use Vendored OpenAL
    set(ALSOFT_UTILS FALSE CACHE BOOL "" FORCE)
    set(ALSOFT_EXAMPLES FALSE CACHE BOOL "" FORCE)
    set(ALSOFT_TESTS FALSE CACHE BOOL "" FORCE)
    set(ALSOFT_REQUIRE_OPENSL TRUE CACHE BOOL "" FORCE)
    add_subdirectory(../../../thirdparty/OpenAL openal EXCLUDE_FROM_ALL)
    target_link_libraries(reminecraftpe-sound PUBLIC OpenAL::OpenAL)
elseif(EMSCRIPTEN)
    # Use Emscripten's OpenAL
    target_link_libraries(reminecraftpe-sound PUBLIC openal)
else()
    # Use System OpenAL
    find_package(OpenAL REQUIRED)
    target_link_libraries(reminecraftpe-sound PUBLIC "${OPENAL_LIBRARY}")
    target_include_directories(reminecraftpe-sound PUBLIC "${OPENAL_INCLUDE_DIR}")
endif()

# Headers
target_include_directories(reminecraftpe-sound PUBLIC .)
