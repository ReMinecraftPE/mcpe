cmake_minimum_required(VERSION 3.16.0)
project(reminecraftpe-ios)

target_compile_definitions(reminecraftpe-core PUBLIC HANDLE_CHARS_SEPARATELY USE_GLES)
target_compile_definitions(stb_image PUBLIC STBI_NO_THREAD_LOCALS)

# Build
add_executable(reminecraftpe
    AppPlatform_iOS.mm
    EAGLView.m
    main.m
    minecraftpeAppDelegate.mm
    minecraftpeViewController.mm
    ShowKeyboardView.mm
)

# Core
target_link_libraries(reminecraftpe reminecraftpe-core)

# stb_image
target_link_libraries(reminecraftpe stb_image)

# libobjc
target_link_libraries(reminecraftpe objc)

# frameworks
target_link_libraries(reminecraftpe
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
    target_compile_definitions(reminecraftpe PUBLIC FEATURE_MENU_BACKGROUND)
endif()
if(EXISTS "${MC_ROOT}/game/assets/environment/clouds.png")
    target_compile_definitions(reminecraftpe PUBLIC FEATURE_CLOUDS)
endif()
if(EXISTS "${MC_ROOT}/game/assets/misc/grasscolor.png")
    target_compile_definitions(reminecraftpe PUBLIC FEATURE_GRASS_COLOR)
endif()
if(EXISTS "${MC_ROOT}/game/assets/misc/foliagecolor.png")
    target_compile_definitions(reminecraftpe PUBLIC FEATURE_FOLIAGE_COLOR)
endif()
