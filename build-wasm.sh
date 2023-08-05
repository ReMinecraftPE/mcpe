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
./emsdk install 3.1.42 # https://github.com/emscripten-core/emscripten/issues/19921
./emsdk activate 3.1.42 > /dev/null

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
emcmake cmake -GNinja "$@" ../../platforms/sdl

# Build
cmake --build .

# Bundle
cp reminecraftpe.* ../dist
cp ../../platforms/sdl/wasm_shell.html ../dist/reminecraftpe.html
cp ../../thirdparty/coi-serviceworker/coi-serviceworker.min.js ../dist
