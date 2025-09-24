project(reminecraftpe-platforms)

# Select & Build
set(DEFAULT_PLATFORM "sdl2")
if(MCPE_WIN32)
    set(DEFAULT_PLATFORM "windows")
endif()
set(REMCPE_PLATFORM "${DEFAULT_PLATFORM}" CACHE STRING "ReMCPE Platform (Check <root>/platforms)")
message(STATUS "Selected Platform: ${REMCPE_PLATFORM}")

# Add platform-specific subdirectory
if(REMCPE_PLATFORM STREQUAL "sdl1")
    add_subdirectory("sdl/sdl1")
elseif(REMCPE_PLATFORM STREQUAL "sdl2")
    add_subdirectory("sdl/sdl2")
else()
    add_subdirectory("${REMCPE_PLATFORM}")
endif()

# Load Sound
add_subdirectory(sound)
