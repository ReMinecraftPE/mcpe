#!/bin/sh
# shellcheck disable=2086
set -e

[ "${0%/*}" = "$0" ] && scriptroot="." || scriptroot="${0%/*}"
cd "$scriptroot"

targets='i386-apple-macos10.7 x86_64-apple-macos10.7 arm64-apple-macos11.0'
# Must be kept in sync with the cmake executable name
bin='reminecraftpe'

platformdir=$PWD

workdir="$PWD/build/work"
arm64_sdk="$workdir/arm64-mac-sdk"
x86_sdk="$workdir/x86-mac-sdk"
mkdir -p "$workdir"
cd "$workdir"

# Increase this if we ever make a change to the SDK, for example
# using a newer SDK version, and we need to invalidate the cache.
sdkver=1
if ! [ -d "$x86_sdk" ] || ! [ -d "$arm64_sdk" ] || [ "$(cat sdkver 2>/dev/null)" != "$sdkver" ]; then
    printf '\nDownloading macOS SDKs...\n\n'
    (
    # for arm64
    [ -d "$arm64_sdk" ] && rm -rf "$arm64_sdk"
    wget -q https://github.com/alexey-lysiuk/macos-sdk/releases/download/11.3/MacOSX11.3.tar.bz2
    tar xf MacOSX11.3.tar.bz2
    mv MacOSX11.3.sdk "$arm64_sdk"
    ) &
    (
    # for x86
    [ -d "$x86_sdk" ] && rm -rf "$x86_sdk"
    wget -q https://github.com/alexey-lysiuk/macos-sdk/releases/download/10.9/MacOSX10.9.tar.bz2
    tar xf MacOSX10.9.tar.bz2
    mv MacOSX10.9.sdk "$x86_sdk"
    )
    wait
    rm ./*.tar.bz2
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
    installnametool='install_name_tool'
else
    ar="${AR:-"llvm-ar"}"
    ranlib="${RANLIB:-"llvm-ranlib"}"
    strip='cctools-strip'
    installnametool='llvm-install-name-tool'
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

if [ -n "$outdated_toolchain" ]; then
    # this step is needed even on macOS since newer versions of Xcode will straight up not let you link for old macOS versions anymore
    printf '\nBuilding ld64 and strip...\n\n'

    tapi_commit=640b4623929c923c0468143ff2a363a48665fa54
    rm -rf cctools-port-*
    wget -O- "https://github.com/tpoechtrager/apple-libtapi/archive/$tapi_commit.tar.gz" | tar -xz

    cd "apple-libtapi-$tapi_commit"
    INSTALLPREFIX="$workdir" CC=clang CXX=clang++ ./build.sh && ./install.sh
    cd ..
    rm -rf "apple-libtapi-$tapi_commit"

    cctools_commit=12e2486bc81c3b2be975d3e117a9d3ab6ec3970c
    rm -rf cctools-port-*
    wget -O- "https://github.com/Un1q32/cctools-port/archive/$cctools_commit.tar.gz" | tar -xz

    cd "cctools-port-$cctools_commit/cctools"
    [ -n "$LLVM_CONFIG" ] && llvm_config="--with-llvm-config=$LLVM_CONFIG"
    ./configure --enable-silent-rules --with-libtapi="$workdir" $llvm_config
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
        make CXX=clang++
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
    if printf 'int main(void) {return 0;}' | REMCPE_TARGET=i386-apple-macos10.4 REMCPE_SDK="$x86_sdk" "$platformdir/macos-cc" -xc - -flto -o "$workdir/testout" >/dev/null 2>&1; then
        lto='-DCMAKE_C_FLAGS=-flto -DCMAKE_CXX_FLAGS=-flto'
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
    case ${target%%-*} in
        (i386|x86_64*)
            export REMCPE_SDK="$x86_sdk"
            set --
        ;;
        (arm64*)
            export REMCPE_SDK="$arm64_sdk"
            set -- -DCMAKE_SHARED_LINKER_FLAGS='-framework GameController' -DCMAKE_EXE_LINKER_FLAGS='-undefined dynamic_lookup'
        ;;
        (*)
            echo "Unknown target"
            exit 1
        ;;
    esac

    printf '\nBuilding for %s\n\n' "$target"
    export REMCPE_TARGET="$target"

    mkdir -p "build-$target"
    cd "build-$target"

    cmake "$platformdir/../.." \
        -DCMAKE_BUILD_TYPE="$build" \
        -DCMAKE_SYSTEM_NAME=Darwin \
        -DREMCPE_PLATFORM=sdl2 \
        -DCMAKE_FIND_ROOT_PATH_MODE_LIBRARY=ONLY \
        -DCMAKE_FIND_ROOT_PATH_MODE_INCLUDE=ONLY \
        -DOPENGL_gl_LIBRARY="$REMCPE_SDK/System/Library/Frameworks/OpenGL.framework" \
        -DOPENGL_INCLUDE_DIR="$REMCPE_SDK/System/Library/Frameworks/OpenGL.framework/Headers" \
        -DCMAKE_AR="$(command -v "$ar")" \
        -DCMAKE_RANLIB="$(command -v "$ranlib")" \
        -DCMAKE_C_COMPILER="$platformdir/macos-cc" \
        -DCMAKE_CXX_COMPILER="$platformdir/macos-c++" \
        -DCMAKE_FIND_ROOT_PATH="$REMCPE_SDK/usr" \
        -DCMAKE_SKIP_RPATH=ON \
        -DWERROR="${WERROR:-OFF}" \
        "$@" \
        $lto
    make -j"$ncpus"

    cd ..
done

lipo -create build-*/"$bin" -output "$bin"
lipo -create build-*/libSDL2-2.0.0.dylib -output libSDL2-2.0.0.dylib
[ -z "$DEBUG" ] && [ -z "$NOSTRIP" ] && {
    "$strip" -no_code_signature_warning "$bin"
    "$strip" -no_code_signature_warning -x libSDL2-2.0.0.dylib
}
"$installnametool" -change libSDL2-2.0.0.dylib '@executable_path/libSDL2-2.0.0.dylib' "$bin"
if command -v ldid >/dev/null; then
    ldid -S "$bin" libSDL2-2.0.0.dylib
else
    codesign -s - "$bin" libSDL2-2.0.0.dylib
fi

mkdir -p ../ReMCPE
cp -a "$platformdir/../../game/assets" ../ReMCPE
mv "$bin" libSDL2-2.0.0.dylib ../ReMCPE
