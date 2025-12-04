#!/bin/sh
# shellcheck disable=2016
set -e

ipaname='ReMCPE.ipa'
# must be kept in sync with the cmake executable name
bin='reminecraftpe'

[ "${0%/*}" = "$0" ] && scriptroot="." || scriptroot="${0%/*}"
cd "$scriptroot/../.."

if ! [ -f "build/$bin" ]; then
    printf 'Expected working binary at build/%s.\n' "$bin"
    printf 'Please do a cmake build before running this script.\n'
    exit 1
fi

if ! command -v plistutil >/dev/null; then
    printf 'plistutil not found!\n'
    exit 1
fi

rm -rf platforms/ios/build/ipa
apppath='platforms/ios/build/ipa/Payload/ReMCPE.app'
mkdir -p "$apppath"
cp "build/$bin" "$apppath/minecraftpe"
sed -E -e 's|\$\{EXECUTABLE_NAME\}|minecraftpe|' -e 's|\$\{PRODUCT_NAME(:rfc1034identifier)?\}|minecraftpe|g' platforms/ios/minecraftpe-Info.plist |
    plistutil -o "$apppath/Info.plist" -f bin
cd game/assets
apppath="../../$apppath"
if [ -f font/default.png ]; then
    cp font/default.png "$apppath/default8.png"
elif [ -f font/default8.png ]; then
    cp font/default8.png "$apppath/default8.png"
fi
cp icon.png "$apppath/Icon.png" || true
cp -a \
    ../../platforms/ios/precompiled/* \
    gui/*.png \
    mob/* \
    item/* \
    sound/* \
    sounds/random/* \
    app/launch/* \
    patches/* \
    terrain.png \
    particles.png \
    "$apppath" || true
cd ../../platforms/ios/build/ipa
rm -f "../$ipaname"
zip -r "../$ipaname" Payload

printf '\nDone! Your IPA is at platforms/ios/build/%s\n' "$ipaname"
