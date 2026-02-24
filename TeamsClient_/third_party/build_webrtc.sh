#!/usr/bin/env bash
set -e


mkdir -p webrtc
cd webrtc

# ----------------------------
# Configuration paths
# ----------------------------

ROOT_DIR="$(pwd)"
DEPOT_TOOLS_DIR="$ROOT_DIR/depot_tools"
WEBRTC_SRC_DIR="$ROOT_DIR/src"
OUT_DIR="$WEBRTC_SRC_DIR/out/Release"

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
# Patch BUILD.gn (force builtin video codec factories)
# ----------------------------
BUILD_GN="$WEBRTC_SRC_DIR/BUILD.gn"

echo "Patching BUILD.gn (builtin video codec factories)..."

if ! grep -q 'builtin_video_encoder_factory' "$BUILD_GN"; then
  awk '
    BEGIN { in_webrtc=0; in_deps=0 }
    {
      print $0
      if ($0 ~ /rtc_static_library\("webrtc"\)/) {
        in_webrtc=1
      }
      if (in_webrtc && $0 ~ /^[[:space:]]*deps = \[/) {
        in_deps=1
        print "      \"api/video_codecs:builtin_video_encoder_factory\","
        print "      \"api/video_codecs:builtin_video_decoder_factory\","
      }
      if (in_deps && $0 ~ /^[[:space:]]*\]/) {
        in_deps=0
        in_webrtc=0
      }
    }
  ' "$BUILD_GN" > "${BUILD_GN}.patched"

  mv "${BUILD_GN}.patched" "$BUILD_GN"
  echo "BUILD.gn patched successfully"
else
  echo "BUILD.gn already patched – skipping"
fi

# ----------------------------
# Génération GN
# ----------------------------
echo "Generating GN build files..."
gn gen "$OUT_DIR" --args='
use_custom_libcxx=false
is_debug=false
is_component_build=false

target_os="mac"
target_cpu="arm64"

rtc_include_tests=false
rtc_build_examples=false
rtc_build_tools=false
rtc_build_libvpx=true

rtc_use_h264=true
proprietary_codecs=true
ffmpeg_branding="Chrome"
'
# rtc_include_internal_video_codecs=true # Inutile normalement, a voir au prochain build
# A adapter en fonction du proco


# ----------------------------
# Build Ninja
# ----------------------------
echo "Building WebRTC..."
ninja -C "$OUT_DIR" webrtc

# ----------------------------
# Export headers et lib
# ----------------------------
echo "Exporting headers and lib..."

mkdir -p "$LIB_DIR"
cp "$OUT_DIR/obj/libwebrtc.a" "$LIB_DIR/libwebrtc.a"

echo "== WebRTC build finished (macOS) =="
echo "Headers: $INCLUDE_DIR"
echo "Lib: $LIB_DIR/libwebrtc.a"