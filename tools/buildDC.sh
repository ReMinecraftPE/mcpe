# Helps compile for DC, just streamlines the lengthy commands.
cd ../
mkdir build
cd build
clear && kos-cmake .. -DREMCPE_PLATFORM=sdl1 -DREMCPE_SOUND_PLATFORM=dummy -DUSE_GL_VBO_EMULATION=1 -DOPENAL_LIBRARY=/opt/toolchains/dc/kos-ports/libAL/inst/lib/libAL.a -DZLIB_INCLUDE_DIR=/opt/toolchains/dc/kos-ports/zlib/inst/include && make clean -j12 && make -j12
