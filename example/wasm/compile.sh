#!/usr/bin/env bash

# compile dependencies
cd .conan
conan install \
    -s build_type=Release \
    -s compiler=clang \
    -s compiler.version=4.0 \
    -s compiler.libcxx=libstdc++11 \
    --build=missing
cd ..

# compile index.cpp using Emscripten (see https://github.com/juj/emsdk)
emcc src/index.cpp \
  --shell-file src/shell.html \
  -o public/index.html \
  @.conan/conanbuildinfo.gcc \
  -O2 \
  -s WASM=1 \
  -s USE_SDL=2 \
  -s LEGACY_GL_EMULATION=1 \
  --std=c++14 \
  --bind
