cmake_minimum_required(VERSION 3.16.0)
project(reminecraftpe)

# Store Output In Top-Level Build Directory
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

if (WIN32 OR LINUX)
set(CMAKE_CXX_STANDARD 11)
else()
set(CMAKE_CXX_STANDARD 98)
endif()
set(CXX_EXTENSIONS OFF)

# Warnings
add_compile_options(-Wall)
if(WERROR)
    add_compile_options(-Werror)
endif()

# Detect Windows
include(CheckSymbolExists)
check_symbol_exists("_WIN32" "" REMCPE_WIN32)

# Clang
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-Wno-inconsistent-missing-override)
endif()
if(APPLE)
    add_compile_options(-Wno-deprecated-declarations)
endif()

# Platform-specific stuff #
if(REMCPE_WIN32) # Windows
    add_link_options(
        -static-libgcc
        -static-libstdc++
    )
elseif(XENON) # Xbox 360 (libXenon)
    include_directories(${DEVKITXENON}/usr/include)
    link_directories(${DEVKITXENON}/usr/lib ${DEVKITXENON}/xenon/lib/32)
    set(CMAKE_EXE_LINKER_FLAGS "-n -T ${XENON_LDSCRIPT}")

    # Binary to Object/Header
    function(xenon_bin2o TARGET_NAME BIN_FILE)
        get_filename_component(BIN_NAME ${BIN_FILE} NAME)
        string(REPLACE "." "_" SAFE_NAME ${BIN_NAME})
        # If it starts with a number, prepend underscore
        if(SAFE_NAME MATCHES "^[0-9]")
            set(SAFE_NAME "_${SAFE_NAME}")
        endif()

        set(ASM_FILE "${CMAKE_CURRENT_BINARY_DIR}/${BIN_NAME}.s")
        set(HEADER_FILE "${CMAKE_CURRENT_BINARY_DIR}/${SAFE_NAME}.h")

        # Generate Header
        file(WRITE ${HEADER_FILE} 
            "extern const u8 ${SAFE_NAME}_end[];\n"
            "extern const u8 ${SAFE_NAME}[];\n"
            "extern const u32 ${SAFE_NAME}_size;\n"
        )

        # Run bin2s to create assembly
        add_custom_command(
            OUTPUT ${ASM_FILE}
            COMMAND bin2s ${CMAKE_CURRENT_SOURCE_DIR}/${BIN_FILE} > ${ASM_FILE}
            DEPENDS ${BIN_FILE}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        )

        # Add to target sources
        target_sources(${TARGET_NAME} PRIVATE ${ASM_FILE})
        target_include_directories(${TARGET_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
    endfunction()

    #add_subdirectory(thirdparty/libxenon_utils EXCLUDE_FROM_ALL)
elseif(EMSCRIPTEN) # WASM
    function(add_compile_and_link_options)
        add_compile_options(${ARGV})
        add_link_options(${ARGV})
    endfunction()
    set(CMAKE_EXECUTABLE_SUFFIX ".js")
    add_link_options("$<$<CONFIG:DEBUG>:-gsource-map>")
    add_link_options(-Wno-pthreads-mem-growth -sALLOW_MEMORY_GROWTH=1)
elseif(HAIKU) # HaikuOS
    link_libraries(network)
elseif(ANDROID) # Android
    link_libraries(log)
endif()

# Threading
if(EMSCRIPTEN)
    add_compile_and_link_options(-pthread)
else()
    find_package(Threads)
    link_libraries(Threads::Threads)
endif()

# Disable thread local storage (needed for C++98 & iOS)
add_compile_definitions(RAPIDJSON_NO_THREAD_LOCAL STBI_NO_THREAD_LOCALS)

if(NOT DEFINED REMCPE_GFX_API)
	message(STATUS "No GFX API specified, defaulting to OGL...")
	set(REMCPE_GFX_API "OGL" CACHE STRING "HAL GFX API")
endif()

message(STATUS "Selected GFX API: ${REMCPE_GFX_API}")

# stb_image And Other Libraries
add_subdirectory(thirdparty/stb_image EXCLUDE_FROM_ALL)

# Load Common Code
add_subdirectory(source)

# Load Platform-Specific Code
add_subdirectory(platforms)

# Assets
if(EMSCRIPTEN)
    target_link_options(reminecraftpe PRIVATE --use-preload-plugins --preload-file "${CMAKE_CURRENT_SOURCE_DIR}/game@/")
elseif(XENON)
    # idk, we'll figure it out later
    # Example: adding a binary file (like a texture or sound)
    # xenon_bin2o(my_app.elf data/font.ttf)
else()
    file(CREATE_LINK "${CMAKE_CURRENT_SOURCE_DIR}/game/assets" "${CMAKE_CURRENT_BINARY_DIR}/assets" SYMBOLIC)
endif()

# VBO Emulation
if(USE_GL_VBO_EMULATION)
    add_compile_definitions(USE_GL_VBO_EMULATION)
endif()

# Disable warnings on third party libraries
target_compile_options(stb_vorbis PRIVATE -w)
target_compile_options(stb_image PRIVATE -w)
target_compile_options(raknet PRIVATE -w)
