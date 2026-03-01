project(nbcraft-platforms)

# Select & Build
set(DEFAULT_PLATFORM "sdl2")
if(NBC_WIN32)
    set(DEFAULT_PLATFORM "windows")
endif()
set(NBC_PLATFORM "${DEFAULT_PLATFORM}" CACHE STRING "NBCraft Platform (Check <root>/platforms)")
message(STATUS "Selected Platform: ${NBC_PLATFORM}")

# Add platform-specific subdirectory
if(NBC_PLATFORM STREQUAL "sdl1")
    add_subdirectory("sdl/sdl1")
elseif(NBC_PLATFORM STREQUAL "sdl2")
    add_subdirectory("sdl/sdl2")
else()
    add_subdirectory("${NBC_PLATFORM}")
endif()

# Load Audio
add_subdirectory(audio)

# Load Input
add_subdirectory(input)