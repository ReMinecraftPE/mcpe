#!/bin/sh
# shellcheck disable=2086
set -e

[ "${0%/*}" = "$0" ] && scriptroot="." || scriptroot="${0%/*}"
cd "$scriptroot"

arch="${ARCH:-x86_64}"
target="$arch-w64-mingw32"
# Must be kept in sync with the cmake executable name
bin='nbcraft.exe'

platformdir=$PWD

workdir="$PWD/build/work"
mkdir -p "$workdir"
cd "$workdir"

if command -v nproc >/dev/null; then
    ncpus="$(nproc)"
else
    ncpus="$(sysctl -n hw.ncpu)"
fi

for dep in make cmake; do
    if ! command -v "$dep" >/dev/null; then
        printf '%s not found!\n' "$dep"
        exit 1
    fi
done

export PATH="$PWD/toolchain-$arch/bin:$PATH"

# Increase this if we ever make a change to the toolchain, for example
# using a newer GCC version, and we need to invalidate the cache.
toolchainver=1
if [ "$(cat "toolchain-$arch/toolchainver" 2>/dev/null)" != "$toolchainver" ]; then
    # adapted from https://github.com/DiscordMessenger/dm/blob/master/doc/pentium-toolchain/README.md

    case $arch in
        (x86_64|i?86)
            winnt=0x0501 # Windows XP
        ;;
        (arm64)
            winnt=0x0A00 # Windows 10
        ;;
        (*)
            printf 'Unknown architecture!\n'
            exit 1
        ;;
    esac

    rm -rf "toolchain-$arch"
    printf '\nBuilding %s toolchain...\n\n' "$arch"

    binutils_version='2.46.0'
    rm -rf binutils-*
    wget -O- "https://ftp.gnu.org/gnu/binutils/binutils-$binutils_version.tar.xz" | tar -xJ

    cd "binutils-$binutils_version"
    ./configure \
        --prefix="$workdir/toolchain-$arch" \
        --target="$target" \
        --disable-multilib \
        CFLAGS='-Wno-discarded-qualifiers'
    make -j"$ncpus"
    make -j"$ncpus" install-strip
    cd ..
    rm -rf "binutils-$binutils_version" &

    mingw_version='13.0.0'
    rm -rf mingw-w64-*
    wget -O- "https://sourceforge.net/projects/mingw-w64/files/mingw-w64/mingw-w64-release/mingw-w64-v$mingw_version.tar.bz2/download" | tar -xj

    cd "mingw-w64-v$mingw_version/mingw-w64-headers"
    ./configure \
        --host="$target" \
        --prefix="$workdir/toolchain-$arch/$arch-w64-mingw32" \
        --with-default-win32-winnt="$winnt" \
        --with-default-msvcrt=msvcrt-os
    make -j"$ncpus" install
    cd ../..

    gcc_version='15.2.0'
    rm -rf gcc-*
    wget -O- "https://ftp.gnu.org/gnu/gcc/gcc-$gcc_version/gcc-$gcc_version.tar.xz" | tar -xJ

    cd "gcc-$gcc_version"
    mkdir build
    cd build
    set --
    [ -n "$GMP" ] && set -- --with-gmp="$GMP"
    [ -n "$MPFR" ] && set -- "$@" --with-mpfr="$MPFR"
    [ -n "$MPC" ] && set -- "$@" --with-mpc="$MPC"
    ../configure \
        --prefix="$workdir/toolchain-$arch" \
        --target="$target" \
        --disable-shared \
        --disable-libstdcxx-time \
        --disable-libstdcxx-filesystem-ts \
        --disable-libgcov \
        --disable-libgomp \
        --disable-multilib \
        --disable-nls \
        --with-system-zlib \
        --enable-languages=c,c++ \
        "$@"
    make -j"$ncpus" all-gcc
    make -j"$ncpus" install-strip-gcc
    cd ../..

    cd "mingw-w64-v$mingw_version/mingw-w64-crt"
    ./configure \
        --host="$target" \
        --prefix="$workdir/toolchain-$arch/$arch-w64-mingw32" \
        --with-default-win32-winnt="$winnt" \
        --with-default-msvcrt=msvcrt-os
    make -j1
    make -j1 install
    cd ../..
    rm -rf "mingw-w64-v$mingw_version" &

    cd "gcc-$gcc_version/build"
    make -j"$ncpus"
    make -j"$ncpus" install-strip
    cd ../..
    rm -rf "gcc-$gcc_version" &

    printf '%s' "$toolchainver" > "toolchain-$arch/toolchainver"
    outdated_toolchain=1
    wait
fi

if [ -n "$DEBUG" ]; then
    build=Debug
else
    build=Release
fi

# Delete old build files if build settings change or if the SDK changes.
printf '%s\n' "$DEBUG" > buildsettings
if [ -n "$outdated_toolchain" ] ||
    ! cmp -s buildsettings lastbuildsettings; then
    rm -rf build-*
fi
mv buildsettings lastbuildsettings

printf '\nBuilding for %s\n\n' "$arch"

mkdir -p "build-$arch"
cd "build-$arch"

cmake "$platformdir/../.." \
    -DCMAKE_BUILD_TYPE="$build" \
    -DCMAKE_SYSTEM_NAME=Windows \
    -DCMAKE_C_COMPILER="$target-gcc" \
    -DCMAKE_CXX_COMPILER="$target-g++" \
    -DCMAKE_EXE_LINKER_FLAGS='-static' \
    -DWERROR="${WERROR:-OFF}" \
    "$@"
make -j"$ncpus"

cd ..

rm -rf ../NBCraft
mkdir -p ../NBCraft

cp -a "$platformdir/../../game/assets" ../NBCraft
cp "build-$arch/$bin" ../NBCraft
[ -z "$DEBUG" ] && [ -z "$NOSTRIP" ] &&
    "$target-strip" "../NBCraft/$bin"
