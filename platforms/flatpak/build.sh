#!/bin/sh
set -e

# cd to the directory this script is in
[ "${0%/*}" = "$0" ] && scriptroot="." || scriptroot="${0%/*}"
cd "$scriptroot"

bundleid='io.github.reminecraftpe'

platformdir=$PWD

if command -v nproc >/dev/null; then
    ncpus="$(nproc)"
else
    ncpus="$(sysctl -n hw.ncpu)"
fi

rm -rf build
mkdir -p build/work
cd build

flatpak build-init output "$bundleid" org.freedesktop.Sdk org.freedesktop.Platform 25.08

cd work

fpbuild() {
    flatpak build ../output "$@"
}

fpbuild cmake "$platformdir/../.." \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=ON \
    -DREMCPE_PLATFORM=sdl2 \
    -DREMCPE_GFX_API=OGL
fpbuild cmake --build . --parallel "$ncpus"
fpbuild mkdir -p /app/bin
fpbuild strip reminecraftpe
fpbuild cp reminecraftpe /app/bin
fpbuild cp -a "$(readlink assets)" /app
fpbuild cp "$platformdir/rungame.sh" /app/bin

cd ..

flatpak build-finish output \
    --share=ipc \
    --socket=fallback-x11 \
    --socket=wayland \
    --device=dri \
    --device=input \
    --share=network \
    --command=rungame.sh

flatpak build-export repo output
flatpak build-bundle repo ReMCPE.flatpak "$bundleid"
