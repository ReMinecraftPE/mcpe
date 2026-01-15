project(reminecraftpe-input)

# Select & Build
if(REMCPE_PLATFORM STREQUAL "sdl1")
elseif(REMCPE_PLATFORM STREQUAL "sdl2")
elseif(REMCPE_PLATFORM STREQUAL "windows")
    add_subdirectory("xinput")
	target_link_libraries(reminecraftpe reminecraftpe-input)
endif()

