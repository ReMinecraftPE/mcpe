#!/bin/bash

set -e

# Working Directory
mkdir -p wasm
cd wasm

# Clone Emscripten SDK
if [ ! -d emsdk ]; then
    git clone https://github.com/emscripten-core/emsdk.git
fi
cd emsdk

# Update Emscripten SDK
git pull
./emsdk install latest
./emsdk activate latest > /dev/null

# Use Emscripten SDK
export EMSDK_QUIET=1
source ./emsdk_env.sh

# Create Output Directory
cd ../
rm -rf dist
mkdir dist

# Create Build Directory
mkdir -p build
cd build

# Configure Build
emcmake cmake -GNinja "$@" ../../

# Build
cmake --build .

# Bundle
cp reminecraftpe.* ../dist
cp ../../platforms/sdl/sdl2/emscripten/wasm_shell.html ../dist/reminecraftpe.html
cp ../../thirdparty/coi-serviceworker/coi-serviceworker.min.js ../dist
