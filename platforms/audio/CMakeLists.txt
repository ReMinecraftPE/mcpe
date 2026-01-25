project(reminecraftpe-audio)

# Select & Build
set(DEFAULT_AUDIO_LIBRARY "openal")
if(REMCPE_WIN32)
    set(DEFAULT_AUDIO_LIBRARY "directsound")
endif()
set(REMCPE_AUDIO_LIBRARY "${DEFAULT_AUDIO_LIBRARY}" CACHE STRING "ReMCPE Audio Library (Check <root>/platforms/audio)")
message(STATUS "Selected Audio Library: ${REMCPE_AUDIO_LIBRARY}")
add_subdirectory("${REMCPE_AUDIO_LIBRARY}")
target_link_libraries(reminecraftpe reminecraftpe-audio)
