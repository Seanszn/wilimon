#!/usr/bin/env bash
set -e

cmake -DCMAKE_TOOLCHAIN_FILE="${WASI_SDK_PATH}/share/cmake/wasi-sdk.cmake" \
      -B build .

cmake --build build

