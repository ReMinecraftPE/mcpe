project(nbcraft-input)

# Select & Build
if(NBC_PLATFORM STREQUAL "sdl1")
elseif(NBC_PLATFORM STREQUAL "sdl2")
elseif(NBC_PLATFORM STREQUAL "windows")
    add_subdirectory("xinput")
	target_link_libraries(nbcraft nbcraft-input)
endif()

