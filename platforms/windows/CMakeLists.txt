cmake_minimum_required(VERSION 3.16.0)
project(reminecraftpe-windows)

# Win32 Build
target_compile_definitions(reminecraftpe-core
    PUBLIC HANDLE_CHARS_SEPARATELY
)

# Libraries
target_link_libraries(reminecraftpe-core
    PUBLIC opengl32
    PUBLIC glu32
    PUBLIC gdi32
    PUBLIC uuid
)

# Build
add_executable(reminecraftpe WIN32
    LoggerWin32.cpp
    AppPlatform_win32.cpp
    main.cpp
)

# Core
target_link_libraries(reminecraftpe reminecraftpe-core)

# stb_image (If Needed)
target_link_libraries(reminecraftpe stb_image)