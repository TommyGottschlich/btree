#!/bin/bash

# Clean script for btree project
# This script removes build artifacts
# Run from the repo root as: ./support/clean.sh

echo "Cleaning 2-3tree project..."

BUILD_BASE="build"

# Remove build directories
if [ -d "$BUILD_BASE/arm64" ]; then
    echo "Removing $BUILD_BASE/arm64 directory..."
    rm -rf "$BUILD_BASE/arm64"
fi

if [ -d "$BUILD_BASE/x86" ]; then
    echo "Removing $BUILD_BASE/x86 directory..."
    rm -rf "$BUILD_BASE/x86"
fi

echo "Clean completed!"
