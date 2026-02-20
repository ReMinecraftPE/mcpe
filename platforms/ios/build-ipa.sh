#!/bin/sh
# shellcheck disable=2016
set -e

ipaname='ReMCPE.ipa'
# must be kept in sync with the cmake executable name
bin="${1:-build/reminecraftpe}"
# must be kept in sync with the info.plist
execname='minecraftpe'

platformdir='platforms/ios'
builddir="$platformdir/build"
assetdir='game/assets'
ipadir="$builddir/ipa"
apppath="$ipadir/Payload/minecraftpe.app"

[ "${0%/*}" = "$0" ] && scriptroot="." || scriptroot="${0%/*}"
cd "$scriptroot/../.."

if ! [ -f "$bin" ]; then
    printf 'Expected working binary at %s.\n' "$bin"
    printf 'Please do a cmake build before running this script.\n'
    exit 1
fi

if ! command -v plistutil >/dev/null; then
    printf 'plistutil not found!\n'
    exit 1
fi

rm -rf "$ipadir"
mkdir -p "$apppath"
cp "$bin" "$apppath/$execname"
sed -E -e "s|\\\$\{EXECUTABLE_NAME\}|$execname|" -e "s|\\\$\{PRODUCT_NAME(:rfc1034identifier)?\}|$execname|g" "$platformdir/minecraftpe-Info.plist" |
    plistutil -o "$apppath/Info.plist" -f bin
cp -a \
    "$platformdir/precompiled"/* \
    "$assetdir" \
    "$apppath" || true
mv "$apppath/assets/app/icon/icon.png" "$apppath/assets/app/launch"/* "$apppath"
rm -rf "$apppath/assets/app" "$apppath/assets/.gitignore"
cd "$ipadir"
rm -f "../$ipaname"
zip -r "../$ipaname" Payload

printf '\nDone! Your IPA is at %s/%s\n' "$builddir" "$ipaname"
