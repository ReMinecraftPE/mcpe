project(reminecraftpe-platforms)

# Select & Build
set(DEFAULT_PLATFORM "sdl")
if(WIN32)
    set(DEFAULT_PLATFORM "windows")
endif()
set(REMCPE_PLATFORM "${DEFAULT_PLATFORM}" CACHE STRING "ReMCPE Platform (Check <root>/platforms)")
add_subdirectory("${REMCPE_PLATFORM}")

# Load Sound
add_subdirectory(sound)