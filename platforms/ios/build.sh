#!/bin/sh
set -e

[ "${0%/*}" = "$0" ] && scriptroot="." || scriptroot="${0%/*}"
cd "$scriptroot"

# We could build for armv6, but we don't due to unplayable performance.
targets='armv7-apple-ios3 arm64-apple-ios7'
# Must be kept in sync with the cmake executable name
bin='reminecraftpe'

printf '\nDownloading iOS SDK...\n\n'

rm -rf build
mkdir -p build/work
cd build/work
workdir="$PWD"

# The iOS 8 SDK supports arm64, armv7s, and armv7 and is small.
# It also doesn't use tbd stubs so we don't need to link ld64 with libtapi.
sdk="$workdir/ios-sdk" # must be kept in sync with the -isysroot arguement in ios-cc.sh
wget https://invoxiplaygames.uk/sdks/iPhoneOS8.0.sdk.tar.lzma
tar xf iPhoneOS8.0.sdk.tar.lzma
mv iPhoneOS8.0.sdk "$sdk"
rm iPhoneOS8.0.sdk.tar.lzma

# Make nproc work on macOS and BSDs
nproc() {
    cmd="$(command -v nproc)"
    if [ -f "$cmd" ]; then
        command nproc
    else
        sysctl -n hw.ncpu
    fi
}

if [ "$(uname -s)" = "Darwin" ]; then
    ar="${AR:-ar}"
    lipo="${LIPO:-lipo}"
    ranlib="${RANLIB:-ranlib}"
    strip='strip'
else
    ar="${AR:-"llvm-ar"}"
    lipo="${LIPO:-"llvm-lipo"}"
    ranlib="${RANLIB:-"llvm-ranlib"}"
    strip='cctools-strip'
fi

for var in ar lipo ranlib; do
    dep="$(eval "echo \$$var")"
    if ! command -v "$dep" >/dev/null; then
        printf '%s not found!\n' "$dep"
        exit 1
    fi
done

export REMCPE_IOS_BUILD=1

mkdir bin
export PATH="$PWD/bin:$PATH"

[ -n "$CLANG" ] && ln -s "$(command -v "$CLANG")" bin/clang && ln -s clang bin/clang++

printf '\nBuilding ld64 and strip...\n\n'

# this step is needed even on macOS since newer versions of Xcode will straight up not let you link for old iOS versions anymore

cctools_commit=35dcdf0285e0a07a32799be3dc08980b6f05313c
wget -O- "https://github.com/tpoechtrager/cctools-port/archive/$cctools_commit.tar.gz" | tar -xz

cd "cctools-port-$cctools_commit/cctools"
./configure --enable-silent-rules
make -C ld64 -j"$(nproc)"
mv ld64/src/ld/ld ../../bin/ld64.ld64
make -C libmacho -j"$(nproc)"
make -C libstuff -j"$(nproc)"
make -C misc strip
cp misc/strip ../../bin/cctools-strip
cd ../..
for target in $targets; do
    ln -s ../../../ios-cc.sh "bin/$target-cc"
    ln -s ../../../ios-cc.sh "bin/$target-c++"
done

printf '\nBuilding ldid...\n\n'

ldid_commit=ef330422ef001ef2aa5792f4c6970d69f3c1f478
wget -O- "https://github.com/ProcursusTeam/ldid/archive/$ldid_commit.tar.gz" | tar -xz

cd "ldid-$ldid_commit"
make CXX=clang++
mv ldid ../bin

cd ../../../../..

for target in $targets; do
    printf '\nBuilding for %s\n\n' "$target"

    rm -rf build
    mkdir build
    cd build

    cmake .. \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_SYSTEM_NAME=Darwin \
        -DREMCPE_PLATFORM=ios \
        -DCMAKE_FIND_ROOT_PATH_MODE_LIBRARY=ONLY \
        -DCMAKE_FIND_ROOT_PATH_MODE_INCLUDE=ONLY \
        -DCMAKE_AR="$(command -v "$ar")" \
        -DCMAKE_RANLIB="$(command -v "$ranlib")" \
        -DCMAKE_C_COMPILER="$target-cc" \
        -DCMAKE_CXX_COMPILER="$target-c++" \
        -DCMAKE_FIND_ROOT_PATH="$sdk/usr"
    make -j"$(nproc)"
    "$strip" "$bin"
    mv "$bin" "$workdir/$bin-$target"

    cd ..
done

"$lipo" -create "$workdir/$bin"-* -output "build/$bin"

[ -n "$REMCPE_NO_IPA" ] || "$workdir/../../build-ipa.sh"
