#!/bin/sh
set -e

# cd to the directory this script is in
[ "${0%/*}" = "$0" ] && scriptroot="." || scriptroot="${0%/*}"
cd "$scriptroot"

# when cross compiling you must have qemu-user-static installed
arch="${ARCH:-x86_64}"

bundleid='io.github.reminecraftpe'

platformdir=$PWD

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

rm -rf build
mkdir -p build/work
cd build

flatpak build-init output "$bundleid" "org.freedesktop.Sdk/$arch" "org.freedesktop.Platform/$arch" "${FLATPAK_RUNTIME_VERSION:-25.08}"

cd work

fpbuild() {
    flatpak build ../output "$@"
}

fpbuild cmake "$platformdir/../.." \
    -DCMAKE_BUILD_TYPE="$build" \
    -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=ON \
    -DCMAKE_C_COMPILER_LAUNCHER=ccache \
    -DCMAKE_CXX_COMPILER_LAUNCHER=ccache \
    -DREMCPE_PLATFORM="${REMCPE_PLATFORM:-sdl2}" \
    -DREMCPE_GFX_API="${REMCPE_GFX_API:-OGL}"
fpbuild cmake --build . --parallel "$ncpus"
fpbuild mkdir -p /app/bin /app/libexec
[ -z "$DEBUG" ] && [ -z "$NOSTRIP" ] && fpbuild strip reminecraftpe
fpbuild cp reminecraftpe /app/libexec
fpbuild cp -a "$(readlink assets)" /app
# rungame.sh is just a script that does a cd to the directory with the assets
fpbuild cp "$platformdir/rungame.sh" /app/bin

cd ..

# permissions given (in order of specification):
# X11 (when wayland is unavailable)
# Wayland
# Audio (both input and output)
# GPU access (OpenGL)
# Input devices (game controllers)
# Network access
flatpak build-finish output \
    --share=ipc \
    --socket=fallback-x11 \
    --socket=wayland \
    --socket=pulseaudio \
    --device=dri \
    --device=input \
    --share=network \
    --command=rungame.sh

flatpak build-export repo output
flatpak build-bundle --arch "$arch" repo "ReMCPE-$arch.flatpak" "$bundleid"
