#!/bin/sh
set -e

[ "${0%/*}" = "$0" ] && scriptroot="." || scriptroot="${0%/*}"
cd "$scriptroot"

nproc() {
    if command -v nproc >/dev/null; then
        nproc
    else
        sysctl -n hw.ncpu
    fi
}

if [ "$(uname -s)" != "Darwin" ] && [ -z "$AR" ] && [ -z "$LIPO" ] && [ -z "$CLANG" ]; then
    for dep in llvm-ar llvm-lipo clang; do
        if ! command -v "$dep" >/dev/null; then
            printf '%s not found!\n' "$dep"
            exit 1
        fi
    done
    ar='llvm-ar'
    lipo='llvm-lipo'
    strip='cctools-strip'
else
    ar="${AR:-ar}"
    lipo="${LIPO:-lipo}"
    strip='strip'
fi

export REMCPE_IOS_BUILD=1

rm -rf ios-work
mkdir ios-work
cd ios-work
mkdir bin
export PATH="$PWD/bin:$PATH"

[ -n "$CLANG" ] && ln -s "$(command -v "$CLANG")" bin/clang

printf 'Building ld64...\n'

# this step is needed even on macOS since newer versions of Xcode will straight up not let you link for old iOS versions anymore

cctools_commit=35dcdf0285e0a07a32799be3dc08980b6f05313c
wget -O- "https://github.com/tpoechtrager/cctools-port/archive/$cctools_commit.tar.gz" | tar -xz

cd "cctools-port-$cctools_commit/cctools"
./configure --enable-silent-rules
make -C ld64 -j"$(nproc)"
cp ld64/src/ld/ld ../../bin/arm64-apple-ios7-ld
make -C libmacho -j"$(nproc)"
make -C libstuff -j"$(nproc)"
make -C misc strip
cp misc/strip ../../bin/cctools-strip
cd ../..
ln -s arm64-apple-ios7-ld bin/armv7-apple-ios3-ld
for cc in armv7-apple-ios3-cc armv7-apple-ios3-c++ arm64-apple-ios7-cc arm64-apple-ios7-c++; do
    ln -s ../../ios-cc.sh "bin/$cc"
done

printf 'Downloading iOS SDKs...\n'

wget https://invoxiplaygames.uk/sdks/iPhoneOS8.0.sdk.tar.lzma
tar xf iPhoneOS8.0.sdk.tar.lzma
mv iPhoneOS8.0.sdk arm64-apple-ios7-sdk
rm iPhoneOS8.0.sdk.tar.lzma

ln -s arm64-apple-ios7-sdk armv7-apple-ios3-sdk

rm -rf ../../../build
mkdir ../../../build
cd ../../../build

cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_SYSTEM_NAME=Darwin \
    -DREMCPE_PLATFORM=ios \
    -DCMAKE_FIND_ROOT_PATH_MODE_LIBRARY=ONLY \
    -DCMAKE_FIND_ROOT_PATH_MODE_INCLUDE=ONLY \
    -DCMAKE_AR="$(command -v "$ar")" \
    -DCMAKE_FIND_ROOT_PATH="$PWD/../platforms/ios/ios-work/armv7-apple-ios3-sdk/usr" \
    -DCMAKE_C_COMPILER=armv7-apple-ios3-cc \
    -DCMAKE_CXX_COMPILER=armv7-apple-ios3-c++
make -j"$(nproc)"
"$strip" reminecraftpe
mv reminecraftpe ../platforms/ios/ios-work/reminecraftpe-armv7

cd ..
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
    -DCMAKE_FIND_ROOT_PATH="$PWD/../platforms/ios/ios-work/arm64-apple-ios7-sdk/usr" \
    -DCMAKE_C_COMPILER=arm64-apple-ios7-cc \
    -DCMAKE_CXX_COMPILER=arm64-apple-ios7-c++
make -j"$(nproc)"
"$strip" reminecraftpe
mv reminecraftpe ../platforms/ios/ios-work/reminecraftpe-arm64

"$lipo" -create ../platforms/ios/ios-work/reminecraftpe-arm64 ../platforms/ios/ios-work/reminecraftpe-armv7 -output reminecraftpe

../platforms/ios/ios-ipa.sh
