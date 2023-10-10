cmake_minimum_required(VERSION 3.16.0)
project(reminecraftpe-sdl)

# SDL Build
add_compile_definitions(USE_SDL USE_OPENAL HANDLE_CHARS_SEPARATELY)

# WASM
if(EMSCRIPTEN)
    function(add_compile_and_link_options)
        add_compile_options(${ARGV})
        add_link_options(${ARGV})
    endfunction()
    set(CMAKE_EXECUTABLE_SUFFIX ".js")
endif()

# Clang
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-Wno-inconsistent-missing-override -Wno-enum-compare-switch -Wno-register)
endif()

# Threads
if(EMSCRIPTEN)
    add_compile_and_link_options(-pthread)
else()
    find_package(Threads)
    link_libraries(Threads::Threads)
endif()

# Build
add_executable(reminecraftpe
    main.cpp
)

# Core
add_subdirectory(../../source source)
target_link_libraries(reminecraftpe reminecraftpe-core)

# SDL Base And Platform
add_subdirectory(base)
if(EMSCRIPTEN)
    add_subdirectory(emscripten)
else()
    add_subdirectory(desktop)
endif()
target_link_libraries(reminecraftpe reminecraftpe-sdl-platform)

# SDL
if(TARGET SDL2::SDL2main)
    target_link_libraries(reminecraftpe SDL2::SDL2main)
endif()

# WASM
if(EMSCRIPTEN)
    target_link_options(reminecraftpe PRIVATE -Wno-pthreads-mem-growth)
    target_link_options(reminecraftpe PRIVATE -sALLOW_MEMORY_GROWTH=1)
    # Export Resize Function
    target_link_options(reminecraftpe PRIVATE -sEXPORTED_FUNCTIONS=_main,_resize_from_js -sEXPORTED_RUNTIME_METHODS=ccall)
endif()
