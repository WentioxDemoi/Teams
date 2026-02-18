#!/usr/bin/env bash
set -e

# ----------------------------
# Configuration paths
# ----------------------------
ROOT_DIR="$(cd "$(dirname "$0")" && pwd)"
DEPOT_TOOLS_DIR="$ROOT_DIR/depot_tools"
WEBRTC_SRC_DIR="$ROOT_DIR/src"
OUT_DIR="$WEBRTC_SRC_DIR/out/Release"

INCLUDE_DIR="$ROOT_DIR/include"
LIB_DIR="$ROOT_DIR/lib"

echo "== WebRTC build (macOS) =="
echo "Root: $ROOT_DIR"

# ----------------------------
# Installer depot_tools si nécessaire
# ----------------------------
if [ ! -d "$DEPOT_TOOLS_DIR" ]; then
    echo "Cloning depot_tools..."
    git clone https://chromium.googlesource.com/chromium/tools/depot_tools.git "$DEPOT_TOOLS_DIR"
fi

export PATH="$DEPOT_TOOLS_DIR:$PATH"

# ----------------------------
# Fetch / config WebRTC
# ----------------------------
cd "$ROOT_DIR"

if [ ! -f "$ROOT_DIR/.gclient" ]; then
    echo "Configuring gclient (fetch webrtc)..."
    fetch --nohooks webrtc
else
    echo "gclient already configured"
fi

cd "$WEBRTC_SRC_DIR"

echo "Syncing WebRTC..."
gclient sync

# ----------------------------
# Génération GN
# ----------------------------
echo "Generating GN build files..."
gn gen "$OUT_DIR" --args='
is_debug=false
rtc_include_tests=false
rtc_build_examples=false
rtc_build_tools=false
is_component_build=false
use_custom_libcxx=false
target_os="mac"
target_cpu="arm64"
'
# A adapter en fonction du proco

# ----------------------------
# Build Ninja
# ----------------------------
echo "Building WebRTC..."
ninja -C "$OUT_DIR"

# ----------------------------
# Export headers et lib
# ----------------------------
echo "Exporting headers and lib..."

mkdir -p "$INCLUDE_DIR"
mkdir -p "$LIB_DIR"

# Copier seulement les headers nécessaires
rsync -a --delete "$WEBRTC_SRC_DIR/api/" "$INCLUDE_DIR/api/"
rsync -a --delete "$WEBRTC_SRC_DIR/rtc_base/" "$INCLUDE_DIR/rtc_base/"

# Copier la lib statique
cp "$OUT_DIR/obj/libwebrtc.a" "$LIB_DIR/libwebrtc.a"

echo "== WebRTC build finished (macOS) =="
echo "Headers: $INCLUDE_DIR"
echo "Lib: $LIB_DIR/libwebrtc.a"