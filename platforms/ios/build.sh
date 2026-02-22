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
sdk="$workdir/sdks/ios-sdk"
export REMCPE_SDK="$sdk"
mkdir -p "$workdir/sdks"
cd "$workdir"

# Increase this if we ever make a change to the SDK, for example
# using a newer SDK version, and we need to invalidate the cache.
sdkver=1
if ! [ -d "$sdk" ] || [ "$(cat sdks/sdkver 2>/dev/null)" != "$sdkver" ]; then
    # The iOS 8 SDK supports arm64, armv7s, and armv7 and is small.
    # It also doesn't use tbd stubs so we don't need to link ld64 with libtapi.
    printf '\nDownloading iOS SDK...\n\n'
    [ -d "$sdk" ] && rm -rf "$sdk"
    rm -f iPhoneOS8.0.sdk.tar.lzma
    wget https://invoxiplaygames.uk/sdks/iPhoneOS8.0.sdk.tar.lzma
    tar -x --lzma -f iPhoneOS8.0.sdk.tar.lzma
    mv iPhoneOS8.0.sdk "$sdk"
    rm iPhoneOS8.0.sdk.tar.lzma
    printf '%s' "$sdkver" > sdks/sdkver
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

if [ -z "$LLVM_CONFIG" ]; then
    if command -v llvm-config >/dev/null; then
        export LLVM_CONFIG=llvm-config
    else
        export LLVM_CONFIG=false
    fi
fi

# Increase this if we ever make a change to the toolchain, for example
# using a newer cctools-port version, and we need to invalidate the cache.
toolchainver=1
if [ "$(cat toolchain/toolchainver 2>/dev/null)" != "$toolchainver" ]; then
    rm -rf toolchain
    outdated_toolchain=1
fi

# invalidate toolchain cache if settings change
"$LLVM_CONFIG" --version > toolchainsettings
if ! cmp -s toolchainsettings toolchain/lasttoolchainsettings; then
    rm -rf toolchain
    outdated_toolchain=1
fi

mkdir -p toolchain/bin
mv toolchainsettings toolchain/lasttoolchainsettings
export PATH="$PWD/toolchain/bin:$PATH"

if [ -n "$CLANG" ]; then
    ln -sf "$(command -v "$CLANG")" toolchain/bin/clang && ln -sf clang toolchain/bin/clang++
else
    rm -f toolchain/bin/clang toolchain/bin/clang++
fi
# ensure we use ccache for the toolchain build
ccache="$(command -v ccache || true)"
printf '#!/bin/sh\nexec %s clang "$@"\n' "$ccache" > toolchain/bin/remcpe-clang
printf '#!/bin/sh\nexec %s clang++ "$@"\n' "$ccache" > toolchain/bin/remcpe-clang++
chmod +x toolchain/bin/remcpe-clang toolchain/bin/remcpe-clang++

if [ -n "$outdated_toolchain" ]; then
    # this step is needed even on macOS since newer versions of Xcode will straight up not let you link for old iOS versions anymore
    printf '\nBuilding ld64 and strip...\n\n'

    cctools_commit=12e2486bc81c3b2be975d3e117a9d3ab6ec3970c
    rm -rf cctools-port-*
    wget -O- "https://github.com/Un1q32/cctools-port/archive/$cctools_commit.tar.gz" | tar -xz

    cd "cctools-port-$cctools_commit/cctools"
    ./configure \
        --enable-silent-rules \
        --with-llvm-config="$LLVM_CONFIG" \
        CC=remcpe-clang \
        CXX=remcpe-clang++
    make -C ld64 -j"$ncpus"
    strip ld64/src/ld/ld
    mv ld64/src/ld/ld ../../toolchain/bin/ld64.ld64
    make -C libmacho -j"$ncpus"
    make -C libstuff -j"$ncpus"
    make -C misc strip lipo
    strip misc/strip misc/lipo
    cp misc/strip ../../toolchain/bin/cctools-strip
    cp misc/lipo ../../toolchain/bin/lipo
    cd ../..
    rm -rf "cctools-port-$cctools_commit"

    if [ "$(uname -s)" != "Darwin" ] && ! command -v ldid >/dev/null; then
        printf '\nBuilding ldid...\n\n'

        ldid_commit=ef330422ef001ef2aa5792f4c6970d69f3c1f478
        rm -rf ldid-*
        wget -O- "https://github.com/ProcursusTeam/ldid/archive/$ldid_commit.tar.gz" | tar -xz

        cd "ldid-$ldid_commit"
        make CXX=remcpe-clang++
        strip ldid
        mv ldid ../toolchain/bin
        cd ..
        rm -rf "ldid-$ldid_commit"
    fi
    printf '%s' "$toolchainver" > toolchain/toolchainver
fi

# checks if the linker we build successfully linked with LLVM and supports LTO,
# and enables LTO in the cmake build if it does.
if [ -z "$DEBUG" ]; then
    if printf 'int main(void) {return 0;}' | REMCPE_TARGET=armv7-apple-ios3.1 "$platformdir/ios-cc" -xc - -flto -o "$workdir/testout" >/dev/null 2>&1; then
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
printf '%s\n' "$DEBUG" > buildsettings
clang -v >> buildsettings 2>&1
if [ -n "$outdated_sdk" ] || ! cmp -s buildsettings lastbuildsettings; then
    rm -rf build-*
fi
mv buildsettings lastbuildsettings

for target in $targets; do
    printf '\nBuilding for %s\n\n' "$target"
    export REMCPE_TARGET="$target"

    mkdir -p "build-$target"
    cd "build-$target"

    cmake "$platformdir/../.." \
        -DCMAKE_BUILD_TYPE="$build" \
        -DCMAKE_SYSTEM_NAME=Darwin \
        -DREMCPE_PLATFORM=ios \
        -DCMAKE_FIND_ROOT_PATH_MODE_PACKAGE=ONLY \
        -DCMAKE_FIND_ROOT_PATH_MODE_LIBRARY=ONLY \
        -DCMAKE_FIND_ROOT_PATH_MODE_INCLUDE=ONLY \
        -DCMAKE_AR="$(command -v "$ar")" \
        -DCMAKE_RANLIB="$(command -v "$ranlib")" \
        -DCMAKE_C_COMPILER="$platformdir/ios-cc" \
        -DCMAKE_CXX_COMPILER="$platformdir/ios-c++" \
        -DCMAKE_FIND_ROOT_PATH="$REMCPE_SDK/usr" \
        -DCMAKE_C_FLAGS="$cflags" \
        -DCMAKE_CXX_FLAGS="$cflags" \
        -DWERROR="${WERROR:-OFF}"
    make -j"$ncpus"

    cd ..
done

lipo -create build-*/"$bin" -output "$bin"
[ -z "$DEBUG" ] && [ -z "$NOSTRIP" ] && "$strip" -no_code_signature_warning "$bin"
if command -v ldid >/dev/null; then
    ldid -S"$entitlements" "$bin"
else
    codesign -f -s - --entitlements "$entitlements" "$bin"
fi

[ -n "$REMCPE_NO_IPA" ] || "$workdir/../../build-ipa.sh" "$PWD/$bin"
