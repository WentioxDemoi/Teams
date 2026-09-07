#!/usr/bin/env bash
set -euo pipefail

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
DEPOT_TOOLS_COMMIT="6fbb6012d6138844379a2df23eea5e86a2ea6696"

if [ ! -d "$DEPOT_TOOLS_DIR" ]; then
    echo "Cloning depot_tools..."
    git clone https://chromium.googlesource.com/chromium/tools/depot_tools.git "$DEPOT_TOOLS_DIR"
fi

cd "$DEPOT_TOOLS_DIR"

git fetch origin "$DEPOT_TOOLS_COMMIT"
git checkout "$DEPOT_TOOLS_COMMIT"

export PATH="$DEPOT_TOOLS_DIR:$PATH"

# ----------------------------
# Fetch / config WebRTC
# ----------------------------
WEBRTC_COMMIT="c4f5b6ef7e5711428a6e433eb7c8d9d3593e7e1a"

cd "$ROOT_DIR"

if [ ! -f "$ROOT_DIR/.gclient" ]; then
    echo "Configuring gclient..."
    fetch --nohooks webrtc
fi

cd "$WEBRTC_SRC_DIR"

git fetch origin "$WEBRTC_COMMIT"
git checkout "$WEBRTC_COMMIT"

cd "$ROOT_DIR"

echo "Syncing WebRTC..."
gclient sync

cd "$WEBRTC_SRC_DIR"


# ----------------------------
# Patch BUILD.gn (include builtin video codec factories)
# ----------------------------
BUILD_GN="$WEBRTC_SRC_DIR/BUILD.gn"

echo "Patching BUILD.gn (builtin video codec factories)..."

# Le path fonctionne uniquement sur le commit de WebRTC.
# Les factories sont des cibles distinctes dans ce checkout; les cibles
# template ne fournissent pas CreateBuiltinVideo*Factory().
if ! grep -q 'api/video_codecs:builtin_video_encoder_factory' "$BUILD_GN"; then
  awk '
    BEGIN { in_webrtc=0; in_deps=0 }
    {
      print $0
      if ($0 ~ /rtc_static_library\("webrtc"\)/) {
        in_webrtc=1
      }
      if (in_webrtc && $0 ~ /^[[:space:]]*deps = \[/) {
        in_deps=1
        print "      \"api/video:adapted_video_track_source\","
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

    if ! grep -q 'api/video_codecs:builtin_video_encoder_factory' "$BUILD_GN" || \
      ! grep -q 'api/video_codecs:builtin_video_decoder_factory' "$BUILD_GN"; then
    echo "ERROR: BUILD.gn patch failed — expected insertion point not found (structure du fichier a peut-être changé pour ce commit)"
    exit 1
  fi

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
use_rtti=true
treat_warnings_as_errors=false
'
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


LIBWEBRTC_PATH="$(find "$OUT_DIR" -name 'libwebrtc.a' -print -quit)"

if [ -z "$LIBWEBRTC_PATH" ]; then
  echo "ERROR: libwebrtc.a introuvable dans $OUT_DIR — la cible n'a peut-être pas produit cette lib pour ce commit"
  exit 1
fi

echo "Found libwebrtc.a at: $LIBWEBRTC_PATH"
cp "$LIBWEBRTC_PATH" "$LIB_DIR/libwebrtc.a"

echo "== WebRTC build finished (macOS) =="
echo "Lib: $LIB_DIR/libwebrtc.a"