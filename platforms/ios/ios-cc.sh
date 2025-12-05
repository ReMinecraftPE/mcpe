#!/bin/sh

if [ -z "$REMCPE_IOS_BUILD" ]; then
    printf 'Do not run this script directly!\n'
    exit 1
fi

case $0 in
    *++) cc="clang++" ;;
    *)   cc="clang"   ;;
esac

target="${0##*/}"
target="${target%-*}"

exec "$cc" \
    -mlinker-version=762 \
    -stdlib=libstdc++ \
    "$@" \
    -isysroot "${0%/*}/../ios-sdk" \
    -target "$target" \
    -fuse-ld=ld64 \
    -Wno-unused-command-line-argument
