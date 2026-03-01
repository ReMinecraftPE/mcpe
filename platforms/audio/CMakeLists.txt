project(nbcraft-audio)

# Select & Build
set(DEFAULT_AUDIO_LIBRARY "openal")
if(NBC_WIN32)
    set(DEFAULT_AUDIO_LIBRARY "directsound")
endif()
set(NBC_AUDIO_LIBRARY "${DEFAULT_AUDIO_LIBRARY}" CACHE STRING "NBCraft Audio Library (Check <root>/platforms/audio)")
message(STATUS "Selected Audio Library: ${NBC_AUDIO_LIBRARY}")
add_subdirectory("${NBC_AUDIO_LIBRARY}")
target_link_libraries(nbcraft nbcraft-audio)
