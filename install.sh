#!/usr/bin/env bash

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${BUILD_DIR:-${ROOT_DIR}/build}"
BUILD_TYPE="${BUILD_TYPE:-Release}"
PREFIX="${PREFIX:-${ROOT_DIR}/dist}"

DEFAULT_PARALLEL=8
if command -v nproc >/dev/null 2>&1; then
  DEFAULT_PARALLEL="$(nproc)"
elif command -v sysctl >/dev/null 2>&1; then
  if SYSCTL_PARALLEL="$(sysctl -n hw.logicalcpu 2>/dev/null)"; then
    DEFAULT_PARALLEL="$SYSCTL_PARALLEL"
  fi
fi
PARALLEL="${PARALLEL:-${DEFAULT_PARALLEL}}"

cmake -S "$ROOT_DIR" -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE="$BUILD_TYPE" -DCMAKE_INSTALL_PREFIX="$PREFIX"
cmake --build "$BUILD_DIR" --parallel "$PARALLEL" --config "$BUILD_TYPE"
cmake --install "$BUILD_DIR" --config "$BUILD_TYPE"

printf 'Installed Smash++ binaries to %s/bin\n' "$PREFIX"
