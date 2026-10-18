#!/usr/bin/env bash
# Tomato Media Player - Environment Verification Script
set -e

echo "=========================================================="
echo "  🍅 Tomato Media Player - Environment Verification"
echo "=========================================================="

# 1. OS & Architecture
OS="$(uname -s)"
ARCH="$(uname -m)"
echo "[+] Operating System: $OS ($ARCH)"

# 2. Compilers & Toolchains
echo -n "[+] Checking C++ Compiler... "
if command -v clang++ >/dev/null 2>&1; then
    CXX_VER=$(clang++ --version | head -n 1)
    echo "Found Clang ($CXX_VER)"
elif command -v g++ >/dev/null 2>&1; then
    CXX_VER=$(g++ --version | head -n 1)
    echo "Found GCC ($CXX_VER)"
else
    echo "WARNING: No C++ compiler (clang++ or g++) detected!"
fi

echo -n "[+] Checking CMake... "
if command -v cmake >/dev/null 2>&1; then
    CMAKE_VER=$(cmake --version | head -n 1)
    echo "Found ($CMAKE_VER)"
else
    echo "WARNING: CMake is not installed!"
fi

# 3. LibVLC Discovery
echo "[+] Probing for LibVLC SDK..."
LIBVLC_FOUND=0
LIBVLC_INC=""
LIBVLC_LIB=""

# Check macOS VLC.app bundle
if [ -d "/Applications/VLC.app/Contents/MacOS/include/vlc" ] && [ -f "/Applications/VLC.app/Contents/MacOS/lib/libvlc.dylib" ]; then
    LIBVLC_INC="/Applications/VLC.app/Contents/MacOS/include"
    LIBVLC_LIB="/Applications/VLC.app/Contents/MacOS/lib/libvlc.dylib"
    LIBVLC_PLUGINS="/Applications/VLC.app/Contents/MacOS/plugins"
    LIBVLC_FOUND=1
    echo "    ✓ Detected LibVLC inside /Applications/VLC.app"
    echo "      Headers: $LIBVLC_INC"
    echo "      Library: $LIBVLC_LIB"
    echo "      Plugins: $LIBVLC_PLUGINS"
# Check pkg-config
elif pkg-config --exists libvlc 2>/dev/null; then
    LIBVLC_INC=$(pkg-config --cflags-only-I libvlc | sed 's/-I//g')
    LIBVLC_LIB=$(pkg-config --libs libvlc)
    LIBVLC_FOUND=1
    echo "    ✓ Detected LibVLC via pkg-config"
    echo "      Headers: $LIBVLC_INC"
    echo "      Library: $LIBVLC_LIB"
# Check standard Linux paths
elif [ -d "/usr/include/vlc" ] && [ -f "/usr/lib/libvlc.so" ]; then
    LIBVLC_INC="/usr/include"
    LIBVLC_LIB="/usr/lib/libvlc.so"
    LIBVLC_FOUND=1
    echo "    ✓ Detected LibVLC in /usr"
fi

if [ "$LIBVLC_FOUND" -eq 1 ]; then
    echo "=========================================================="
    echo "  🎉 Environment Ready for Tomato Build & Test!"
    echo "=========================================================="
    echo "To configure CMake, run:"
    echo "  cmake -B build -S . -DLIBVLC_INCLUDE_DIR=\"$LIBVLC_INC\" -DLIBVLC_LIBRARY=\"$LIBVLC_LIB\""
    echo "To set the runtime plugin path on macOS:"
    echo "  export VLC_PLUGIN_PATH=\"$LIBVLC_PLUGINS\""
else
    echo "=========================================================="
    echo "  ⚠️  LibVLC SDK not found automatically."
    echo "  Please install VLC or libvlc-dev / brew install vlc"
    echo "=========================================================="
    exit 1
fi
