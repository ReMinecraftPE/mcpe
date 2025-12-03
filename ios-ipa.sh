#!/bin/sh
# shellcheck disable=2016
set -e

[ "${0%/*}" = "$0" ] && scriptroot="." || scriptroot="${0%/*}"
cd "$scriptroot"

if ! [ -f build/reminecraftpe ]; then
    printf 'Expected working binary at build/reminecraftpe.\n'
    printf 'Please do a cmake build before running this script.'
    exit 1
fi

for dep in ldid plistutil; do
    if ! command -v "$dep" >/dev/null; then
        printf '%s must be in $PATH to run this script.\n' "$dep"
        exit 1
    fi
done

rm -rf ios-ipa
apppath='ios-ipa/Payload/ReMCPE.app'
mkdir -p "$apppath"
cp build/reminecraftpe "$apppath/minecraftpe"
ldid -Splatforms/ios/minecraftpe.entitlements "$apppath/minecraftpe"
sed -E -e 's|\$\{EXECUTABLE_NAME\}|minecraftpe|' -e 's|\$\{PRODUCT_NAME(:rfc1034identifier)?\}|minecraftpe|g' platforms/ios/minecraftpe-Info.plist |
    plistutil -o "$apppath/Info.plist" -f bin
cp -a \
    platforms/ios/precompiled/* \
    game/assets/images/mob/char.png \
    "$apppath"
cd ios-ipa
zip -r ../ReMCPE.ipa Payload

printf '\nDone! Your IPA is at ReMCPE.ipa.\n'
