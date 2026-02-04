cmake_minimum_required(VERSION 3.16.0)
project(reminecraftpe-windows)

# Win32 Build
target_compile_definitions(reminecraftpe-core
    PUBLIC HANDLE_CHARS_SEPARATELY
)

# Libraries
target_link_libraries(reminecraftpe-core
    PUBLIC gdi32
    PUBLIC uuid
)

if(REMCPE_GFX_API STREQUAL "OGL")
    target_link_libraries(reminecraftpe-core
        PUBLIC opengl32
        PUBLIC glu32
    )
elseif(REMCPE_GFX_API STREQUAL "D3D11")
    target_link_libraries(reminecraftpe-core
        PUBLIC d3d11
        PUBLIC d3dcompiler
        PUBLIC dxgi
        PUBLIC dxguid
    )
endif()

# Build
add_executable(reminecraftpe WIN32
    LoggerWin32.cpp
    AppPlatform_win32.cpp
    main.cpp
    icon.rc
)

# Core
target_link_libraries(reminecraftpe reminecraftpe-core)

# stb_image (If Needed)
target_link_libraries(reminecraftpe stb_image)
