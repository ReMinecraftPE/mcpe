project(reminecraftpe-sound)

# Select & Build
set(DEFAULT_SOUND_PLATFORM "openal")
if(MCPE_WIN32)
    set(DEFAULT_SOUND_PLATFORM "directsound")
endif()
set(REMCPE_SOUND_PLATFORM "${DEFAULT_SOUND_PLATFORM}" CACHE STRING "ReMCPE Sound Platform (Check <root>/platforms/sound)")
message(STATUS "Selected Sound Platform: ${REMCPE_SOUND_PLATFORM}")
add_subdirectory("${REMCPE_SOUND_PLATFORM}")
target_link_libraries(reminecraftpe reminecraftpe-sound)
