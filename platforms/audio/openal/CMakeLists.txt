cmake_minimum_required(VERSION 3.16.0)
project(nbcraft-audio-openal)

# Build
add_library(nbcraft-audio STATIC
	SoundSystemOAL.cpp
	SoundStreamOAL.cpp
)

# Core
target_link_libraries(nbcraft-audio PUBLIC nbcraft-core)

# OpenAL
if(ANDROID)
    # Use Vendored OpenAL
    set(ALSOFT_UTILS FALSE CACHE BOOL "" FORCE)
    set(ALSOFT_EXAMPLES FALSE CACHE BOOL "" FORCE)
    set(ALSOFT_TESTS FALSE CACHE BOOL "" FORCE)
    set(ALSOFT_REQUIRE_OPENSL TRUE CACHE BOOL "" FORCE)
    add_subdirectory(../../../thirdparty/OpenAL openal EXCLUDE_FROM_ALL)
    target_link_libraries(nbcraft-audio PUBLIC OpenAL::OpenAL)
elseif(EMSCRIPTEN)
    # Use Emscripten's OpenAL
    target_link_libraries(nbcraft-audio PUBLIC openal)
elseif(APPLE)
    # Use System OpenAL Framework
    target_link_libraries(nbcraft-audio PUBLIC "-framework OpenAL")
elseif(NINTENDO_SWITCH)
    # Use devkitpro's OpenAL
    target_link_libraries(nbcraft-audio PUBLIC openal)
else()
    # Use System OpenAL
    find_package(OpenAL REQUIRED)
    target_link_libraries(nbcraft-audio PUBLIC "${OPENAL_LIBRARY}")
    target_include_directories(nbcraft-audio PUBLIC "${OPENAL_INCLUDE_DIR}")
endif()

# Headers
target_include_directories(nbcraft-audio PUBLIC .)
