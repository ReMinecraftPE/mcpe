#!/bin/sh
# shellcheck disable=2016
set -e

ipaname='ReMCPE.ipa'
# must be kept in sync with the cmake executable name
bin='reminecraftpe'
# must be kept in sync with the info.plist
execname='minecraftpe'

platformdir='platforms/ios'
builddir="$platformdir/build"
assetdir='game/assets'
ipadir='ipa'
apppath="$builddir/$ipadir/Payload/ReMCPE.app"

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

rm -rf "$builddir/$ipadir"
mkdir -p "$apppath"
cp "build/$bin" "$apppath/$execname"
sed -E -e "s|\$\{EXECUTABLE_NAME\}|$execname|" -e "s|\$\{PRODUCT_NAME(:rfc1034identifier)?\}|$execname|g" "$platformdir/minecraftpe-Info.plist" |
    plistutil -o "$apppath/Info.plist" -f bin
cd "$assetdir"
apppath="../../$apppath"
if [ -f font/default.png ]; then
    cp font/default.png "$apppath/default8.png"
elif [ -f font/default8.png ]; then
    cp font/default8.png "$apppath/default8.png"
fi
cp icon.png "$apppath/Icon.png" || true
cp -a \
    "../../$platformdir/precompiled"/* \
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
cd "../../$builddir/$ipadir"
rm -f "../$ipaname"
zip -r "../$ipaname" Payload

printf '\nDone! Your IPA is at %s/%s\n' "$builddir" "$ipaname"
