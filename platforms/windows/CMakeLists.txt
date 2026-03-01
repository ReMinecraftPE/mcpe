cmake_minimum_required(VERSION 3.16.0)
project(nbcraft-windows)

# Win32 Build
target_compile_definitions(nbcraft-core
    PUBLIC HANDLE_CHARS_SEPARATELY
)

# Libraries
target_link_libraries(nbcraft-core
    PUBLIC gdi32
    PUBLIC uuid
)

if(NBC_GFX_API STREQUAL "OGL")
    target_link_libraries(nbcraft-core
        PUBLIC opengl32
        PUBLIC glu32
    )
elseif(NBC_GFX_API STREQUAL "D3D11")
    target_link_libraries(nbcraft-core
        PUBLIC d3d11
        PUBLIC d3dcompiler
        PUBLIC dxgi
        PUBLIC dxguid
    )
endif()

# Build
add_executable(nbcraft WIN32
    LoggerWin32.cpp
    AppPlatform_win32.cpp
    main.cpp
    NBCraftClient.Win32.rc
)

# Core
target_link_libraries(nbcraft nbcraft-core)

# stb_image (If Needed)
target_link_libraries(nbcraft stb_image)
