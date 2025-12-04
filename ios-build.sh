#!/bin/sh
set -e

[ "${0%/*}" = "$0" ] && scriptroot="." || scriptroot="${0%/*}"
cd "$scriptroot"

for dep in llvm-ar llvm-lipo clang; do
    if ! command -v "$dep" >/dev/null; then
        printf '%s not found!\n' "$dep"
        exit 1
    fi
done

export REMCPE_IOS_BUILD=1

rm -rf ios-work
mkdir ios-work
cd ios-work
mkdir bin
export PATH="$PWD/bin:$PATH"

[ -n "$CLANG" ] && ln -s "$(command -v "$CLANG")" bin/clang

cctools_commit=35dcdf0285e0a07a32799be3dc08980b6f05313c
wget -O- "https://github.com/tpoechtrager/cctools-port/archive/$cctools_commit.tar.gz" | tar -xz

cd "cctools-port-$cctools_commit/cctools"
./configure
make -C ld64 -j"$(nproc)"
cp ld64/src/ld/ld ../../bin/armv6-apple-ios3-ld
make -C libmacho -j"$(nproc)"
make -C libstuff -j"$(nproc)"
make -C misc strip
cp misc/strip ../../bin/cctools-strip
cd ../..
ln -s armv6-apple-ios3-ld bin/arm64-apple-ios7-ld
for cc in armv6-apple-ios3-cc armv6-apple-ios3-c++ arm64-apple-ios7-cc arm64-apple-ios7-c++; do
    ln -s ../../ios-cc.sh "bin/$cc"
done
ln -s "$(command -v llvm-ar)" bin/armv6-apple-ios3-ar
ln -s armv6-apple-ios3-ar bin/arm64-apple-ios7-ar

wget https://invoxiplaygames.uk/sdks/iPhoneOS8.0.sdk.tar.lzma
tar xf iPhoneOS8.0.sdk.tar.lzma
mv iPhoneOS8.0.sdk arm64-apple-ios7-sdk
rm iPhoneOS8.0.sdk.tar.lzma

wget https://invoxiplaygames.uk/sdks/iPhoneOS5.0.sdk.tar.lzma
tar xf iPhoneOS5.0.sdk.tar.lzma
mv iPhoneOS5.0.sdk armv6-apple-ios3-sdk
rm iPhoneOS5.0.sdk.tar.lzma

rm -rf ../build
mkdir ../build
cd ../build

cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_SYSTEM_NAME=Darwin \
    -DREMCPE_PLATFORM=ios \
    -DCMAKE_FIND_ROOT_PATH_MODE_LIBRARY=ONLY \
    -DCMAKE_FIND_ROOT_PATH_MODE_INCLUDE=ONLY \
    -DCMAKE_FIND_ROOT_PATH="$PWD/../ios-work/armv6-apple-ios3-sdk/usr" \
    -DCMAKE_C_COMPILER=armv6-apple-ios3-cc \
    -DCMAKE_CXX_COMPILER=armv6-apple-ios3-c++
make -j"$(nproc)"
cctools-strip reminecraftpe
mv reminecraftpe ../ios-work/reminecraftpe-armv6

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
    -DCMAKE_FIND_ROOT_PATH="$PWD/../ios-work/arm64-apple-ios7-sdk/usr" \
    -DCMAKE_C_COMPILER=arm64-apple-ios7-cc \
    -DCMAKE_CXX_COMPILER=arm64-apple-ios7-c++
make -j"$(nproc)"
cctools-strip reminecraftpe
mv reminecraftpe ../ios-work/reminecraftpe-arm64

llvm-lipo -create ../ios-work/reminecraftpe-arm64 ../ios-work/reminecraftpe-armv6 -output reminecraftpe

../ios-ipa.sh
