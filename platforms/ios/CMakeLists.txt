cmake_minimum_required(VERSION 3.16.0)
project(nbcraft-ios)

target_compile_definitions(nbcraft-core PUBLIC HANDLE_CHARS_SEPARATELY USE_GLES)

# Build
add_executable(nbcraft
    AppPlatform_iOS.mm
    EAGLView.m
    main.m
    NBCAppDelegate.mm
    NBCViewController.mm
    ShowKeyboardView.mm
)

# Core
target_link_libraries(nbcraft nbcraft-core)

# stb_image
target_link_libraries(nbcraft stb_image)

# libobjc
target_link_libraries(nbcraft objc)

# frameworks
target_link_libraries(nbcraft
    "-framework CoreGraphics"
    "-framework QuartzCore"
    "-framework Foundation"
    "-framework AVFoundation"
    "-framework UIKit"
    "-framework OpenGLES"
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../source/client/app)

# Check for the presence of some optional asset based features.
if(EXISTS "${MC_ROOT}/game/assets/gui/background/panorama_0.png")
    target_compile_definitions(nbcraft PUBLIC FEATURE_MENU_BACKGROUND)
endif()
if(EXISTS "${MC_ROOT}/game/assets/environment/clouds.png")
    target_compile_definitions(nbcraft PUBLIC FEATURE_CLOUDS)
endif()
if(EXISTS "${MC_ROOT}/game/assets/misc/grasscolor.png")
    target_compile_definitions(nbcraft PUBLIC FEATURE_GRASS_COLOR)
endif()
if(EXISTS "${MC_ROOT}/game/assets/misc/foliagecolor.png")
    target_compile_definitions(nbcraft PUBLIC FEATURE_FOLIAGE_COLOR)
endif()
