#!/usr/bin/env bash
set -e

# =========================
# Configuration
# =========================
ROOT_DIR="$(cd "$(dirname "$0")" && pwd)"
INSTALL_DIR="$ROOT_DIR/libyuv"
SRC_DIR="$ROOT_DIR/libyuv/libyuv_src"
BUILD_DIR="$SRC_DIR/build"

echo "== Installing libyuv locally =="
echo "Root        : $ROOT_DIR"
echo "Install dir : $INSTALL_DIR"

# =========================
# Clone
# =========================
if [ ! -d "$SRC_DIR" ]; then
  echo "Cloning libyuv..."
  git clone https://chromium.googlesource.com/libyuv/libyuv "$SRC_DIR"
else
  echo "libyuv source already exists"
fi

# =========================
# Build
# =========================
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

echo "Configuring (arm64, Release)..."
cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_OSX_ARCHITECTURES=arm64 \
  -DCMAKE_INSTALL_PREFIX="$INSTALL_DIR"

echo "Building..."
make -j$(sysctl -n hw.ncpu)

echo "Installing..."
make install

# =========================
# Summary
# =========================
echo ""
echo "== libyuv installed =="
echo "Headers : $INSTALL_DIR/include/libyuv"
echo "Library : $INSTALL_DIR/lib"
echo ""