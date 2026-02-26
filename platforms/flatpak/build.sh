#!/bin/sh
set -e

# cd to the directory this script is in
[ "${0%/*}" = "$0" ] && scriptroot="." || scriptroot="${0%/*}"
cd "$scriptroot"

arch="${ARCH:-x86_64}"

bundleid='io.github.reminecraftpe'

platformdir=$PWD

workdir="$PWD/build/work"

if command -v nproc >/dev/null; then
    ncpus="$(nproc)"
else
    ncpus="$(sysctl -n hw.ncpu)"
fi

if [ -n "$DEBUG" ]; then
    build=Debug
else
    build=Release
fi

rm -rf build/output build/work/build
mkdir -p build/work/build
cd build

runtime="${FLATPAK_RUNTIME_VERSION:-25.08}"
[ "$ARCH" = 'i386' ] && runtime='1.6'

flatpak build-init output "$bundleid" "org.freedesktop.Sdk/$arch" "org.freedesktop.Platform/$arch" "$runtime"

cd work

fpbuild() {
    flatpak build ../../output "$@"
}

cmake=cmake
set -- -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=ON

toolchainver=2
if [ "$ARCH" = 'i386' ]; then
    if [ "$(cat toolchain/toolchainver 2>/dev/null)" != "$toolchainver" ]; then
        printf '\nBuilding i386 toolchain...\n\n'

        cmake_version='4.2.3'
        wget -O- "https://github.com/Kitware/CMake/archive/refs/tags/v$cmake_version.tar.gz" | tar -xz
        cd "CMake-$cmake_version"
        fpbuild ./bootstrap \
            --prefix="$workdir/toolchain" \
            --parallel="$ncpus" \
            --enable-ccache \
            --system-zlib \
            --system-bzip2 \
            --system-curl \
            --system-expat \
            --system-liblzma
        fpbuild make -j"$ncpus"
        fpbuild make install/strip -j"$ncpus"
        cd ..
        rm -rf "CMake-$cmake_version"

        rm -rf toolchain/doc share/emacs share/vim share/bash-completion

        printf '%s' "$toolchainver" > toolchain/toolchainver
    fi

    set -- \
        -DREMCPE_VENDORED_SDL2=ON \
        -DCMAKE_C_COMPILER=clang \
        -DCMAKE_CXX_COMPILER=clang++

    cmake="$workdir/toolchain/bin/cmake"
fi

cd build
fpbuild "$cmake" "$platformdir/../.." \
    -DCMAKE_BUILD_TYPE="$build" \
    -DCMAKE_C_COMPILER_LAUNCHER=ccache \
    -DCMAKE_CXX_COMPILER_LAUNCHER=ccache \
    -DREMCPE_PLATFORM="${REMCPE_PLATFORM:-sdl2}" \
    -DREMCPE_GFX_API="${REMCPE_GFX_API:-OGL}" \
    "$@"
fpbuild "$cmake" --build . --parallel "$ncpus"
fpbuild mkdir -p /app/bin /app/libexec
[ -z "$DEBUG" ] && [ -z "$NOSTRIP" ] &&
    fpbuild strip reminecraftpe
fpbuild cp reminecraftpe /app/libexec
fpbuild cp -a "$(readlink assets)" /app
# rungame.sh is just a script that does a cd to the directory with the assets
fpbuild cp "$platformdir/rungame.sh" /app/bin

cd ../..

# permissions given (in order of specification):
# X11 (when wayland is unavailable)
# Wayland
# Audio (both input and output)
# Device access (USB, game controllers, GPU, etc)*
# Network access
#
# *: Flatpak has --device=dri for GPU access, and --device=input for
# input devices (controllers), which would be enough for us, but it was
# added relatively recently, and older flatpak versions don't understand
# it, so we just do --device=all to make everything work on old systems.
flatpak build-finish output \
    --share=ipc \
    --socket=fallback-x11 \
    --socket=wayland \
    --socket=pulseaudio \
    --device=all \
    --share=network \
    --command=rungame.sh

flatpak build-export repo output
flatpak build-bundle --arch "$arch" repo "ReMCPE-$arch.flatpak" "$bundleid"
