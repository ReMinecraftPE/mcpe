#!/bin/sh
# We want a universal binary like system, but
# we want to run the i386 slice on x86_64 Tiger.

# This has to be a shell script because a C program
# cannot reliably get the path of the executable,
# and we need to run other binaries at a relative location
# from this script.

execdir="${0%/*}"
arch="$("$execdir/libexec/arch")"

if [ "$arch" = "x86_64" ]; then
    case $(uname -r) in
        (8.*|9.*|10.*)
            # Tiger, Leopard, or Snow Leopard
            exec "$execdir/libexec/reminecraftpe-i386"
        ;;
        (*)
            exec "$execdir/libexec/reminecraftpe-x86_64"
        ;;
    esac
fi

exec "$execdir/libexec/reminecraftpe-$arch" "$@"
