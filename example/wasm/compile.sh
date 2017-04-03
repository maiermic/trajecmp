#!/usr/bin/env bash

emcc src/index.cpp \
  --shell-file src/shell.html \
  -o public/index.html \
  -I/home/maiermic/.conan/data/Boost/1.60.0/lasote/stable/package/89066e72c51676d91729a0d7fee21c166ca507da/include \
  -I../../src \
  -O2 \
  -s WASM=1 \
  -s USE_SDL=2 \
  -s LEGACY_GL_EMULATION=1 \
  --std=c++14
