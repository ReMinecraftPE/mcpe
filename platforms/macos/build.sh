#!/bin/sh
# shellcheck disable=2086
set -e

[ "${0%/*}" = "$0" ] && scriptroot="." || scriptroot="${0%/*}"
cd "$scriptroot"

# TODO: powerpc
targets='i386-apple-macos10.4 x86_64-apple-macos10.6 arm64-apple-macos11.0'
# Must be kept in sync with the cmake executable name
bin='reminecraftpe'

platformdir=$PWD

workdir="$PWD/build/work"
arm64_sdk="$workdir/arm64-mac-sdk"
x86_64_sdk="$workdir/x86_64-mac-sdk"
old_sdk="$workdir/old-mac-sdk"
mkdir -p "$workdir"
cd "$workdir"

# Increase this if we ever make a change to the SDK, for example
# using a newer SDK version, and we need to invalidate the cache.
sdkver=2
if ! [ -d "$old_sdk" ] || ! [ -d "$arm64_sdk" ] || [ "$(cat sdkver 2>/dev/null)" != "$sdkver" ]; then
    printf '\nDownloading macOS SDKs...\n\n'
    (
    # for new stuff
    [ -d "$arm64_sdk" ] && rm -rf "$arm64_sdk"
    rm -f MacOSX11.0.sdk.tar.xz
    wget -q https://github.com/phracker/MacOSX-SDKs/releases/download/11.3/MacOSX11.0.sdk.tar.xz
    tar -xJf MacOSX11.0.sdk.tar.xz
    mv MacOSX11.0.sdk "$arm64_sdk"
    ) &
    (
    # for x86_64
    [ -d "$x86_64_sdk" ] && rm -rf "$x86_64_sdk"
    rm -f MacOSX10.6.tar.bz2
    wget -q https://github.com/alexey-lysiuk/macos-sdk/releases/download/10.6/MacOSX10.6.tar.bz2
    tar -xjf MacOSX10.6.tar.bz2 2>/dev/null
    mv MacOSX10.6.sdk "$x86_64_sdk"
    ) &
    (
    # for old stuff
    [ -d "$old_sdk" ] && rm -rf "$old_sdk"
    rm -f MacOSX10.5.sdk.tar.xz
    wget -q https://github.com/phracker/MacOSX-SDKs/releases/download/11.3/MacOSX10.5.sdk.tar.xz
    tar -xJf MacOSX10.5.sdk.tar.xz
    mv MacOSX10.5.sdk "$old_sdk"
    # patch the sdk to fix a bug
    sed -e 's/^#if !__LP64__$//' -e 's/^#ifndef __SCSI__$/#if !__LP64__\n#ifndef __SCSI__/' \
        "$old_sdk/System/Library/Frameworks/CoreServices.framework/Frameworks/OSServices.framework/Headers/OSServices.h" \
        > OSServices.h
    mv OSServices.h "$old_sdk/System/Library/Frameworks/CoreServices.framework/Frameworks/OSServices.framework/Headers"
    )
    wait
    rm ./*.tar.xz ./*.tar.bz2
    printf '%s' "$sdkver" > sdkver
    outdated_sdk=1
fi

if command -v nproc >/dev/null; then
    ncpus="$(nproc)"
else
    ncpus="$(sysctl -n hw.ncpu)"
fi

if [ "$(uname -s)" = "Darwin" ]; then
    ar="${AR:-ar}"
    ranlib="${RANLIB:-ranlib}"
    strip='strip'
else
    ar="${AR:-"llvm-ar"}"
    ranlib="${RANLIB:-"llvm-ranlib"}"
    strip='cctools-strip'
fi

for var in ar ranlib; do
    dep="$(eval "echo \$$var")"
    if ! command -v "$dep" >/dev/null; then
        printf '%s not found!\n' "$dep"
        exit 1
    fi
done

for dep in "${CLANG:-clang}" make cmake; do
    if ! command -v "$dep" >/dev/null; then
        printf '%s not found!\n' "$dep"
        exit 1
    fi
done

# Increase this if we ever make a change to the toolchain, for example
# using a newer cctools-port version, and we need to invalidate the cache.
toolchainver=1
if [ "$(cat bin/toolchainver 2>/dev/null)" != "$toolchainver" ]; then
    rm -rf bin
    outdated_toolchain=1
fi

mkdir -p bin
export PATH="$PWD/bin:$PATH"

if [ -n "$CLANG" ]; then
    ln -sf "$(command -v "$CLANG")" bin/clang && ln -sf clang bin/clang++
else
    rm -f bin/clang bin/clang++
fi
# ensure we use ccache for the toolchain build
ccache="$(command -v ccache || true)"
printf '#!/bin/sh\n
        exec %sclang "$@"\n' "$ccache " > bin/remcpe-clang
printf '#!/bin/sh\n
        exec %sclang++ "$@"\n' "$ccache " > bin/remcpe-clang++
chmod +x bin/remcpe-clang bin/remcpe-clang++

if [ -n "$outdated_toolchain" ]; then
    # this step is needed even on macOS since newer versions of Xcode will straight up not let you link for old macOS versions anymore
    printf '\nBuilding ld64 and strip...\n\n'

    tapi_commit=640b4623929c923c0468143ff2a363a48665fa54
    rm -rf cctools-port-*
    wget -O- "https://github.com/tpoechtrager/apple-libtapi/archive/$tapi_commit.tar.gz" | tar -xz

    cd "apple-libtapi-$tapi_commit"
    INSTALLPREFIX="$workdir" CC=remcpe-clang CXX=remcpe-clang++ ./build.sh && ./install.sh
    cd ..
    rm -rf "apple-libtapi-$tapi_commit"

    cctools_commit=12e2486bc81c3b2be975d3e117a9d3ab6ec3970c
    rm -rf cctools-port-*
    wget -O- "https://github.com/Un1q32/cctools-port/archive/$cctools_commit.tar.gz" | tar -xz

    cd "cctools-port-$cctools_commit/cctools"
    if [ -n "$LLVM_CONFIG" ]; then
        set -- --with-llvm-config="$LLVM_CONFIG"
    else
        set --
    fi
    ./configure --enable-silent-rules --with-libtapi="$workdir" CC=remcpe-clang CXX=remcpe-clang++ "$@"
    make -C ld64 -j"$ncpus"
    mv ld64/src/ld/ld ../../bin/ld64.ld64
    make -C libmacho -j"$ncpus"
    make -C libstuff -j"$ncpus"
    make -C misc strip lipo
    cp misc/strip ../../bin/cctools-strip
    cp misc/lipo ../../bin/lipo
    cd ../..
    rm -rf "cctools-port-$cctools_commit"

    if [ "$(uname -s)" != "Darwin" ] && ! command -v ldid >/dev/null; then
        printf '\nBuilding ldid...\n\n'

        ldid_commit=ef330422ef001ef2aa5792f4c6970d69f3c1f478
        rm -rf ldid-*
        wget -O- "https://github.com/ProcursusTeam/ldid/archive/$ldid_commit.tar.gz" | tar -xz

        cd "ldid-$ldid_commit"
        make CXX=remcpe-clang++
        mv ldid ../bin
        cd ..
        rm -rf "ldid-$ldid_commit"
    fi
    rm -rf include
    printf '%s' "$toolchainver" > "$workdir/bin/toolchainver"
fi

# checks if the linker we build successfully linked with LLVM and supports LTO,
# and enables LTO in the cmake build if it does.
if [ -z "$DEBUG" ]; then
    if printf 'int main(void) {return 0;}' | REMCPE_TARGET=i386-apple-macos10.4 REMCPE_SDK="$old_sdk" "$platformdir/macos-cc" -xc - -flto -o "$workdir/testout" >/dev/null 2>&1; then
        cflags='-flto'
    fi
    rm -f "$workdir/testout"
fi

if [ -n "$DEBUG" ]; then
    build=Debug
else
    build=Release
fi

# Delete old build files if build settings change or if the SDK changes.
printf '%s\n%s\n' "$DEBUG" "$CLANG" > buildsettings
if [ -n "$outdated_sdk" ] || ! cmp -s buildsettings lastbuildsettings; then
    rm -rf build-*
fi
mv buildsettings lastbuildsettings

for target in $targets; do
    printf '\nBuilding for %s\n\n' "$target"
    export REMCPE_TARGET="$target"

    mkdir -p "build-$target"
    cd "build-$target"

    arch="${target%%-*}"
    case $arch in
        (i386|x86_64*)
            if [ "$arch" = 'i386' ]; then
                export REMCPE_SDK="$old_sdk"
            else
                export REMCPE_SDK="$x86_64_sdk"
            fi
            set -- -DCMAKE_EXE_LINKER_FLAGS='-framework IOKit -framework Carbon -framework AudioUnit'
            platform='sdl1'
            sdl1ver=1
            if ! [ -f sdl1/lib/libSDL.a ] || [ "$(cat sdl1/sdl1ver 2>/dev/null)" != "$sdl1ver" ]; then
                sdl1_commit=25712c1e9270035667e1ed68f2acc5b82b441461
                rm -rf SDL-1.2-*
                if ! [ -f ../sdl1src.tar.gz ] ||
                    [ "$(sha256sum ../sdl1src.tar.gz | awk '{print $1}')" != 'd1753e9d8fc3d25cabd0198122546a3d53b5b1df0b208145cff5fc4b2d50d786' ]; then
                    wget -O ../sdl1src.tar.gz "https://github.com/libsdl-org/SDL-1.2/archive/$sdl1_commit.tar.gz"
                fi
                tar -xzf ../sdl1src.tar.gz
                cd "SDL-1.2-$sdl1_commit"
                ./configure \
                    --host="$arch-apple-darwin" \
                    --prefix="${PWD%/*}/sdl1" \
                    --disable-shared \
                    --disable-video-x11 \
                    CC="$platformdir/macos-cc" \
                    CXX="$platformdir/macos-c++" \
                    CFLAGS="-O2 $cflags" \
                    CXXFLAGS="-O2 $cflags" \
                    CPPFLAGS='-D__DARWIN_UNIX03=1' \
                    AR="$ar" \
                    RANLIB="$ranlib"
                make -j"$ncpus"
                make install -j"$ncpus"
                cd ..
                printf '%s' "$sdl1ver" > sdl1/sdl1ver
                rm -rf "SDL-1.2-$sdl1_commit"
            fi
            cflags="$cflags -I$PWD/sdl1/include -D__DARWIN_UNIX03=1"
        ;;
        (arm64*)
            export REMCPE_SDK="$arm64_sdk"
            set -- -DCMAKE_EXE_LINKER_FLAGS='-undefined dynamic_lookup'
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
        -DCMAKE_FIND_ROOT_PATH_MODE_LIBRARY=ONLY \
        -DCMAKE_FIND_ROOT_PATH_MODE_INCLUDE=ONLY \
        -DCMAKE_AR="$(command -v "$ar")" \
        -DCMAKE_RANLIB="$(command -v "$ranlib")" \
        -DCMAKE_C_COMPILER="$platformdir/macos-cc" \
        -DCMAKE_CXX_COMPILER="$platformdir/macos-c++" \
        -DCMAKE_FIND_ROOT_PATH="$REMCPE_SDK/usr;$PWD/sdl1" \
        -DCMAKE_SYSROOT="$REMCPE_SDK" \
        -DCMAKE_C_FLAGS="$cflags" \
        -DCMAKE_CXX_FLAGS="$cflags" \
        -DWERROR="${WERROR:-OFF}" \
        "$@"
    make -j"$ncpus"

    cd ..
done

lipo -create build-*/"$bin" -output "$bin"
[ -z "$DEBUG" ] && [ -z "$NOSTRIP" ] && "$strip" -no_code_signature_warning "$bin"
if command -v ldid >/dev/null; then
    ldid -S "$bin"
else
    codesign -f -s - "$bin"
fi

mkdir -p ../ReMCPE
cp -a "$platformdir/../../game/assets" ../ReMCPE
mv "$bin" ../ReMCPE
