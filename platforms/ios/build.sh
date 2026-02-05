#!/bin/sh
# shellcheck disable=2086
set -e

[ "${0%/*}" = "$0" ] && scriptroot="." || scriptroot="${0%/*}"
cd "$scriptroot"

# We could build for armv6, but we don't due to unplayable performance.
targets='armv7-apple-ios3.1 arm64-apple-ios7.0'
# Must be kept in sync with the cmake executable name
bin='reminecraftpe'

platformdir=$PWD
entitlements="$platformdir/minecraftpe.entitlements"

workdir="$PWD/build/work"
sdk="$workdir/ios-sdk" # must be kept in sync with the -isysroot arguement in ios-cc.sh
[ -d "$sdk" ] && mv "$sdk" ios-sdk-backup
[ -d "$workdir/bin" ] && mv "$workdir/bin" bin-backup
rm -rf build
mkdir -p "$workdir"
[ -d ios-sdk-backup ] && mv ios-sdk-backup "$sdk"
[ -d bin-backup ] && mv bin-backup "$workdir/bin"
cd "$workdir"

# Increase this if we ever make a change to the SDK, for example
# using a newer SDK version, and we need to invalidate the cache.
sdkver=1
if ! [ -d "$sdk" ] || [ "$(cat "$sdk/sdkver" 2>/dev/null)" != "$sdkver" ]; then
    # The iOS 8 SDK supports arm64, armv7s, and armv7 and is small.
    # It also doesn't use tbd stubs so we don't need to link ld64 with libtapi.
    printf '\nDownloading iOS SDK...\n\n'
    [ -d "$sdk" ] && rm -rf "$sdk"
    wget https://invoxiplaygames.uk/sdks/iPhoneOS8.0.sdk.tar.lzma
    tar xf iPhoneOS8.0.sdk.tar.lzma
    mv iPhoneOS8.0.sdk "$sdk"
    rm iPhoneOS8.0.sdk.tar.lzma
    printf '%s' "$sdkver" > "$sdk/sdkver"
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

if [ -n "$outdated_toolchain" ]; then
    # this step is needed even on macOS since newer versions of Xcode will straight up not let you link for old iOS versions anymore
    printf '\nBuilding ld64 and strip...\n\n'

    cctools_commit=12e2486bc81c3b2be975d3e117a9d3ab6ec3970c
    wget -O- "https://github.com/Un1q32/cctools-port/archive/$cctools_commit.tar.gz" | tar -xz

    cd "cctools-port-$cctools_commit/cctools"
    [ -n "$LLVM_CONFIG" ] && llvm_config="--with-llvm-config=$LLVM_CONFIG"
    ./configure --enable-silent-rules $llvm_config
    make -C ld64 -j"$ncpus"
    mv ld64/src/ld/ld ../../bin/ld64.ld64
    make -C libmacho -j"$ncpus"
    make -C libstuff -j"$ncpus"
    make -C misc strip lipo
    cp misc/strip ../../bin/cctools-strip
    cp misc/lipo ../../bin/lipo
    cd ../..
    printf '%s' "$toolchainver" > "$workdir/bin/toolchainver"

    if [ "$(uname -s)" != "Darwin" ] && ! command -v ldid >/dev/null; then
        printf '\nBuilding ldid...\n\n'

        ldid_commit=ef330422ef001ef2aa5792f4c6970d69f3c1f478
        wget -O- "https://github.com/ProcursusTeam/ldid/archive/$ldid_commit.tar.gz" | tar -xz

        cd "ldid-$ldid_commit"
        make CXX=clang++
        mv ldid ../bin
        cd ..
    fi
fi

# checks if the linker we build successfully linked with LLVM and supports LTO,
# and enables LTO in the cmake build if it does.
if [ -z "$DEBUG" ]; then
    if printf 'int main(void) {return 0;}' | REMCPE_TARGET=armv7-apple-ios3.1 "$platformdir/ios-cc" -xc - -flto -o "$workdir/testout" >/dev/null 2>&1; then
        lto='-DCMAKE_C_FLAGS=-flto -DCMAKE_CXX_FLAGS=-flto'
    fi
    rm -f "$workdir/testout"
fi

# go to the root of the project
cd ../../../..

if [ -n "$DEBUG" ]; then
    build=Debug
else
    build=Release
fi

for target in $targets; do
    printf '\nBuilding for %s\n\n' "$target"
    export REMCPE_TARGET="$target"

    rm -rf build
    mkdir build
    cd build

    cmake .. \
        -DCMAKE_BUILD_TYPE="$build" \
        -DCMAKE_SYSTEM_NAME=Darwin \
        -DREMCPE_PLATFORM=ios \
        -DCMAKE_FIND_ROOT_PATH_MODE_LIBRARY=ONLY \
        -DCMAKE_FIND_ROOT_PATH_MODE_INCLUDE=ONLY \
        -DCMAKE_AR="$(command -v "$ar")" \
        -DCMAKE_RANLIB="$(command -v "$ranlib")" \
        -DCMAKE_C_COMPILER="$platformdir/ios-cc" \
        -DCMAKE_CXX_COMPILER="$platformdir/ios-c++" \
        -DCMAKE_FIND_ROOT_PATH="$sdk/usr" \
        -DWERROR="${WERROR:-OFF}" \
        $lto
    make -j"$ncpus"
    mv "$bin" "$workdir/$bin-$target"

    cd ..
done

lipo -create "$workdir/$bin"-* -output "build/$bin"
[ -z "$DEBUG" ] && [ -z "$NOSTRIP" ] && "$strip" -no_code_signature_warning "build/$bin"
if command -v ldid >/dev/null; then
    ldid -S"$entitlements" "build/$bin"
else
    codesign -s - --entitlements "$entitlements" "build/$bin"
fi

[ -n "$REMCPE_NO_IPA" ] || "$workdir/../../build-ipa.sh"
