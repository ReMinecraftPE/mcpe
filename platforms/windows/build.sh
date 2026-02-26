#!/bin/sh
# shellcheck disable=2086
set -e

[ "${0%/*}" = "$0" ] && scriptroot="." || scriptroot="${0%/*}"
cd "$scriptroot"

arch="${ARCH:-x86_64}"
target="$arch-w64-mingw32"
# Must be kept in sync with the cmake executable name
bin='reminecraftpe'

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
        (i?86)
            winnt=0x0500 # Windows 2000
        ;;
        (x86_64)
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

    cd "mingw-w64-v$mingw_version"
    patch -fNp1 < ../../../mingw-w64.diff
    cd mingw-w64-headers
    ./configure \
        --host="$target" \
        --prefix="$workdir/toolchain-$arch/$arch-w64-mingw32" \
        --with-default-win32-winnt="$winnt" \
        --with-default-msvcrt=crtdll
    make -j"$ncpus" install
    cd ../..

    gcc_version='13.1.0'
    rm -rf gcc-*
    wget -O- "https://ftp.gnu.org/gnu/gcc/gcc-$gcc_version/gcc-$gcc_version.tar.xz" | tar -xJ

    cd "gcc-$gcc_version"
    patch -fNp1 < ../../../gcc.diff
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
        --prefix="$workdir/toolchain/$arch-w64-mingw32" \
        --with-default-win32-winnt="$winnt" \
        --with-default-msvcrt=crtdll \
        --disable-wchar
    make -j1
    make -j1 install
    cd ../..
    rm -rf "mingw-w64-v$mingw_version" &

    cd "gcc-$gcc_version/build"
    make -j"$ncpus"
    make -j"$ncpus" install-strip
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

exit 0

# Delete old build files if build settings change or if the SDK changes.
printf '%s\n' "$DEBUG" > buildsettings
clang -v >> buildsettings 2>&1
if [ -n "$outdated_toolchain" ] ||
    ! cmp -s buildsettings lastbuildsettings; then
    rm -rf build-*
fi
mv buildsettings lastbuildsettings

for target in $targets; do
    printf '\nBuilding for %s\n\n' "$target"
    export REMCPE_TARGET="$target"

    mkdir -p "build-$target"
    cd "build-$target"

    arch="${target%%-*}"
    cc="$platformdir/macos-cc"
    cxx="$platformdir/macos-c++"
    target_ar="$ar"
    target_ranlib="$ranlib"
    case $arch in
        (i386|powerpc*|ppc*)
            if [ "$arch" = 'i386' ]; then
                target_cflags="$cflags -march=pentium-m"
                set -- -DCMAKE_EXE_LINKER_FLAGS='-framework IOKit -framework Carbon -framework AudioUnit -undefined dynamic_lookup'
            else
                target_cflags=
                cc="$target-gcc"
                cxx="$target-g++"
                target_ar="cctools-ar"
                target_ranlib="cctools-ranlib"
                set -- -DCMAKE_EXE_LINKER_FLAGS='-framework IOKit -framework Carbon -framework AudioUnit -static-libgcc'
            fi
            export REMCPE_SDK="$old_sdk"
            platform='sdl1'
            sdl1ver=1
            if ! [ -f sdl/lib/libSDL.a ] || [ "$(cat sdl/sdl1ver 2>/dev/null)" != "$sdl1ver" ]; then
                sdl1_commit=25712c1e9270035667e1ed68f2acc5b82b441461
                rm -rf SDL-1.2-*
                if ! [ -f ../sdl1src.tar.gz ] ||
                    [ "$(sha256sum ../sdl1src.tar.gz | awk '{print $1}')" != 'd1753e9d8fc3d25cabd0198122546a3d53b5b1df0b208145cff5fc4b2d50d786' ]; then
                    wget -O ../sdl1src.tar.gz "https://github.com/libsdl-org/SDL-1.2/archive/$sdl1_commit.tar.gz"
                fi
                tar -xzf ../sdl1src.tar.gz
                cd "SDL-1.2-$sdl1_commit"
                if [ -n "$DEBUG" ]; then
                    opt='-O0'
                else
                    opt='-O2'
                fi
                if [ "$arch" != 'i386' ]; then
                    sed -e 's/-fpascal-strings//g' configure > configure.patched
                    mv configure.patched configure
                    chmod +x configure
                fi
                ./configure \
                    --host="$arch-apple-darwin" \
                    --prefix="${PWD%/*}/sdl" \
                    --disable-shared \
                    --disable-video-x11 \
                    CC="$cc" \
                    CXX="$cxx" \
                    CFLAGS="$opt $target_cflags" \
                    CXXFLAGS="$opt $target_cflags" \
                    CPPFLAGS='-DNDEBUG' \
                    AR="$target_ar" \
                    RANLIB="$target_ranlib"
                make -j"$ncpus"
                make install -j"$ncpus"
                cd ..
                printf '%s' "$sdl1ver" > sdl/sdl1ver
                rm -rf "SDL-1.2-$sdl1_commit"
            fi
            target_cflags="$target_cflags -I$PWD/sdl/include"
        ;;
        (arm64*|x86_64*)
            target_cflags="$cflags"
            case $arch in
                (arm64*)
                    export REMCPE_SDK="$arm64_sdk"
                    set -- -DCMAKE_EXE_LINKER_FLAGS='-undefined dynamic_lookup'
                ;;
                (x86_64*)
                    export REMCPE_SDK="$x86_64_sdk"
                    set --
                ;;
            esac
            platform='sdl2'
        ;;
        (*)
            echo "Unknown target"
            exit 1
        ;;
    esac

    cmake "$platformdir/../.." \
        -DCMAKE_BUILD_TYPE="$build" \
        -DCMAKE_SYSTEM_NAME=Darwin \
        -DREMCPE_PLATFORM="$platform" \
        -DCMAKE_FIND_ROOT_PATH_MODE_PACKAGE=ONLY \
        -DCMAKE_FIND_ROOT_PATH_MODE_LIBRARY=ONLY \
        -DCMAKE_FIND_ROOT_PATH_MODE_INCLUDE=ONLY \
        -DCMAKE_AR="$(command -v "$target_ar")" \
        -DCMAKE_RANLIB="$(command -v "$target_ranlib")" \
        -DCMAKE_C_COMPILER="$cc" \
        -DCMAKE_CXX_COMPILER="$cxx" \
        -DCMAKE_FIND_ROOT_PATH="$REMCPE_SDK/usr;$PWD/sdl" \
        -DCMAKE_SYSROOT="$REMCPE_SDK" \
        -DCMAKE_C_FLAGS="$target_cflags" \
        -DCMAKE_CXX_FLAGS="$target_cflags" \
        -DWERROR="${WERROR:-OFF}" \
        "$@"
    make -j"$ncpus"

    cd ..
done

rm -rf ../ReMCPE
mkdir -p ../ReMCPE

cp -a "$platformdir/../../game/assets" ../ReMCPE

for target in $targets; do
    arch="${target%%-*}"
    cp "build-$target/$bin" "../ReMCPE/libexec/$bin-$arch"
    case $arch in
        (powerpc*|ppc*) strip='ppc-strip' ;;
        (*) strip='cctools-strip -no_code_signature_warning' ;;
    esac
    [ -z "$DEBUG" ] && [ -z "$NOSTRIP" ] &&
        $strip "../ReMCPE/libexec/$bin-${target%%-*}"
done
