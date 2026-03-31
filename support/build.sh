#!/bin/bash

# Build script for btree project
# This script builds the project using CMake
# Run from the repo root as: ./support/build.sh

set -e  # Exit on any error

echo "Building 2-3tree project..."

# Capture repo root before changing directories
REPO_ROOT="$(pwd)"
PACKAGE_BASE="support/package"

# Detect architecture
ARCH=$(uname -m)
if [ "$ARCH" = "arm64" ]; then
    PACKAGE_DIR="$PACKAGE_BASE/arm64"
    BUILD_DIR="build/arm64"
    echo "Detected architecture: arm64"
else
    PACKAGE_DIR="$PACKAGE_BASE/x86"
    BUILD_DIR="build/x86"
    echo "Detected architecture: x86 ($ARCH)"
fi

# Create build directory if it doesn't exist
if [ ! -d "$BUILD_DIR" ]; then
    echo "Creating build directory..."
    mkdir -p "$BUILD_DIR"
fi

# Run CMake
echo "Running CMake..."
cmake -S "$PACKAGE_DIR" -B "$BUILD_DIR" -DREPO_ROOT="$REPO_ROOT"

# Build the project
echo "Building project..."
cmake --build "$BUILD_DIR"

echo "Build completed successfully!"
echo "You can run the executable with: $BUILD_DIR/2-3tree/2-3tree"
echo "You can run tests with: ctest --test-dir $BUILD_DIR"
