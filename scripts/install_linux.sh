#!/usr/bin/env bash
set -euo pipefail

# Install minimal deps
if ! command -v cmake >/dev/null 2>&1; then echo "Please install cmake >=3.20"; exit 1; fi
if ! command -v git >/dev/null 2>&1; then echo "Please install git"; exit 1; fi

# Optional: curl for GitHub API
if ! command -v curl >/dev/null 2>&1; then echo "[info] curl not found, will fallback to gh CLI if used"; fi

# Build
build_dir="$(pwd)/build"
mkdir -p "$build_dir"
cd "$build_dir"
cmake -DCMAKE_BUILD_TYPE=Release -DENABLE_COVERAGE=OFF ..
cmake --build . --config Release -j
ctest --output-on-failure || true

# Package
cpack || true
