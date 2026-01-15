project(reminecraftpe-input)

# Select & Build
if(REMCPE_WIN32)
    add_subdirectory("xinput")
	target_link_libraries(reminecraftpe reminecraftpe-input)
endif()

