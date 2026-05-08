#!/bin/bash
echo "=== SuperOtto Build Script ==="

if ! command -v cmake &> /dev/null; then
    echo "[ERROR] CMake not found. Install with: brew install cmake / sudo apt install cmake"
    exit 1
fi

echo "Configuring project..."
cmake -B build -DCMAKE_BUILD_TYPE=Release
if [ $? -ne 0 ]; then
    echo "[ERROR] CMake configuration failed. Make sure Qt6 is installed."
    exit 1
fi

echo "Building project..."
cmake --build build -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
if [ $? -ne 0 ]; then
    echo "[ERROR] Build failed."
    exit 1
fi

echo ""
echo "=== Build successful! ==="
echo "Executable: build/SuperOtto"
echo ""
echo "To run, copy resources/music/ and resources/pictures/ next to the exe."
