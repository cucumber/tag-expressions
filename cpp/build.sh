#!/bin/bash

# Build script for Cucumber Tag Expressions C++ library

set -e  # Exit on error

echo "==================================="
echo "Cucumber Tag Expressions C++ Build"
echo "==================================="
echo ""

# Configuration
BUILD_TYPE="${BUILD_TYPE:-Release}"
BUILD_DIR="${BUILD_DIR:-build}"
JOBS="${JOBS:-$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)}"

echo "Build Type: $BUILD_TYPE"
echo "Build Directory: $BUILD_DIR"
echo "Parallel Jobs: $JOBS"
echo ""

# Configure
echo "Configuring CMake..."
cmake -B "$BUILD_DIR" \
    -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

echo ""
echo "Building..."
cmake --build "$BUILD_DIR" -j "$JOBS"

echo ""
echo "Build complete!"
echo ""
echo "Run the example:"
echo "  ./$BUILD_DIR/example"
echo ""
echo "Run tests (if Google Test is available):"
echo "  ./$BUILD_DIR/tag_expressions_test"
echo ""
echo "Install the library:"
echo "  sudo cmake --build $BUILD_DIR --target install"
echo ""
