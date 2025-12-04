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

for dep in ldid plistutil; do
    if ! command -v "$dep" >/dev/null; then
        printf '%s not found!\n' "$dep"
        exit 1
    fi
done

rm -rf platforms/ios/ipa
apppath='platforms/ios/build/ipa/Payload/ReMCPE.app'
mkdir -p "$apppath"
cp "build/$bin" "$apppath/minecraftpe"
ldid -Splatforms/ios/minecraftpe.entitlements "$apppath/minecraftpe"
sed -E -e 's|\$\{EXECUTABLE_NAME\}|minecraftpe|' -e 's|\$\{PRODUCT_NAME(:rfc1034identifier)?\}|minecraftpe|g' platforms/ios/minecraftpe-Info.plist |
    plistutil -o "$apppath/Info.plist" -f bin
if [ -f game/assets/font/default.png ]; then
    cp game/assets/font/default.png "$apppath/default8.png"
elif [ -f game/assets/font/default8.png ]; then
    cp game/assets/font/default8.png "$apppath/default8.png"
fi
cp game/assets/icon.png "$apppath/Icon.png" || true
cp -a \
    platforms/ios/precompiled/* \
    game/assets/gui/*.png \
    game/assets/mob/* \
    game/assets/item/* \
    game/assets/sound/* \
    game/assets/sounds/random/* \
    game/assets/app/launch/* \
    game/assets/patches/* \
    game/assets/terrain.png \
    game/assets/particles.png \
    "$apppath" || true
cd platforms/ios/build/ipa
rm -f "../$ipaname"
zip -r "../$ipaname" Payload

printf '\nDone! Your IPA is at platforms/ios/build/%s\n' "$ipaname"
