# based on https://github.com/Free60Project/libxenon/blob/b52340d905c36a1bfe621d100672e1f30e241a4c/devkitxenon/rules

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR powerpc)

# Check if the environment variable exists
if(NOT DEFINED ENV{DEVKITXENON})
    message(FATAL_ERROR "Please set DEVKITXENON in your environment. export DEVKITXENON=<path to>devkitPPC")
endif()

set(XENON TRUE CACHE BOOL "")
set(DEVKITXENON "$ENV{DEVKITXENON}" CACHE PATH "Path to the libXenon SDK")
#set(LIBXENON_INC "$(DEVKITXENON)/usr/include" CACHE PATH "Path to the libXenon include directory")
#set(LIBXENON_LIB "$(DEVKITXENON)/usr/lib" CACHE PATH "Path to the libXenon lib directory")

set(TOOLCHAIN_PREFIX "xenon")
set(CMAKE_C_COMPILER "${TOOLCHAIN_PREFIX}-gcc")
set(CMAKE_CXX_COMPILER "${TOOLCHAIN_PREFIX}-g++")
set(CMAKE_ASM_COMPILER "${TOOLCHAIN_PREFIX}-gcc") # Use gcc for assembly to handle CPP
set(CMAKE_AR "${TOOLCHAIN_PREFIX}-ar" CACHE FILEPATH "Archiver")
set(CMAKE_OBJCOPY "${TOOLCHAIN_PREFIX}-objcopy" CACHE FILEPATH "Objcopy")
set(CMAKE_STRIP "${TOOLCHAIN_PREFIX}-strip" CACHE FILEPATH "Strip")

set(MACHDEP "-DXENON -m32 -maltivec -fno-pic -mpowerpc64 -mhard-float")
set(CMAKE_C_FLAGS "${MACHDEP}" CACHE STRING "")
set(CMAKE_CXX_FLAGS "${MACHDEP}" CACHE STRING "")
set(CMAKE_ASM_FLAGS "${MACHDEP} -x assembler-with-cpp" CACHE STRING "")

set(XENON_LDSCRIPT "${DEVKITXENON}/app.lds" CACHE STRING "Path to the libXenon linker script")
# Stop CMake from trying to link a full executable during the test phase
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

set(CMAKE_FIND_ROOT_PATH ${DEVKITXENON})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
