# 🛠️ Tomato — Build & Testing Verification Guide

> **Document Version:** 1.0.0  
> **Status:** Empirically Verified Baseline  

---

## 1. System Requirements & Prerequisites

### Minimum Toolchains
- **C++ Compiler:** C++20 compliant compiler
  - macOS: Apple Clang 15.0+ (Xcode Command Line Tools)
  - Linux: GCC 12+ or Clang 15+
  - Windows: MSVC 2022 (v143+) or MinGW-w64 12+
- **Build System:** CMake 3.22 or higher
- **Media Engine SDK:** LibVLC 3.0.18+ or 3.0.20+ (Development headers & shared libraries)
  - macOS: Extracted from `/Applications/VLC.app` or Homebrew
  - Linux: `libvlc-dev` (Debian/Ubuntu) or `vlc-devel` (Fedora/Arch)
  - Windows: Official VideoLAN LibVLC Win64 SDK
- **UI Framework (Phase 1+):** Qt 6.5+ (Qt Quick / QML)

---

## 2. Verified Local Environment (macOS Darwin arm64)

The build and link feasibility has been empirically audited and verified on the local host:

```bash
# Architecture & Kernel
Host: Apple Silicon (arm64)
Kernel: Darwin 25.6.0
C++ Compiler: /usr/bin/clang++ (Apple clang version 17.0.0)
CMake: /opt/homebrew/bin/cmake (3.31.5)
LibVLC Location: /Applications/VLC.app/Contents/MacOS/lib/libvlc.dylib
LibVLC Headers: /Applications/VLC.app/Contents/MacOS/include/vlc/
LibVLC Runtime Version: 3.0.20 Vetinari (3.0.20-0-g6f0d0ab126)
```

### Verified Test Results

Executing the native LibVLC test harness yielded:
```text
Compiling with LibVLC release
Compiled against libvlc version: 3.0.20
Runtime libvlc version: 3.0.20 Vetinari
Runtime libvlc changeset: 3.0.20-0-g6f0d0ab126
Successfully initialized LibVLC instance!
Successfully released LibVLC instance!
```
- **Exit Code:** `0`
- **Memory Violations:** `0`
- **Link Status:** Clean dynamic link with runtime `@rpath` resolution.

---

## 3. Quickstart Build Instructions

### Phase 0: Test Harness & Core Logic

```bash
# 1. Clone or navigate to the repository
cd /path/to/tomato

# 2. Run automated environment check
./scripts/check_env.sh

# 3. Configure CMake
cmake -B build -S . -DCMAKE_BUILD_TYPE=Debug

# 4. Compile the LibVLC test harness and core test targets
cmake --build build --target test_libvlc_harness test_queue_manager

# 5. Run tests
ctest --test-dir build --output-on-failure
```

### Environment Variables
When running standalone binaries linked against an existing VLC installation on macOS, ensure the plugin path is set:
```bash
export VLC_PLUGIN_PATH="/Applications/VLC.app/Contents/MacOS/plugins"
```

---

## 4. Multi-Platform Build Configurations

### macOS (Apple Silicon & Intel)
```bash
cmake -B build -S . \
  -DCMAKE_BUILD_TYPE=Release \
  -DLIBVLC_INCLUDE_DIR="/Applications/VLC.app/Contents/MacOS/include" \
  -DLIBVLC_LIBRARY="/Applications/VLC.app/Contents/MacOS/lib/libvlc.dylib"
cmake --build build -j8
```

### Linux (Ubuntu / Debian / Fedora)
```bash
sudo apt-get install -y cmake g++ libvlc-dev qt6-base-dev qt6-declarative-dev
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
```

### Windows (MSVC 2022)
```powershell
cmake -B build -S . -G "Visual Studio 17 2022" -A x64 `
  -DLIBVLC_SDK_PATH="C:/vlc-3.0.20/sdk"
cmake --build build --config Release
```

---

## 5. Automated Testing Strategy

Tomato employs a three-tier testing hierarchy:

```
┌────────────────────────────────────────────────────────┐
│   Tier 3: End-to-End Playback Integration Tests        │
│   (Real video playback, seek precision, track change)  │
├────────────────────────────────────────────────────────┤
│   Tier 2: LibVLC Engine Adapter Tests                  │
│   (Mock media instances, event manager callbacks)      │
├────────────────────────────────────────────────────────┤
│   Tier 1: Fast Unit Tests                              │
│   (QueueManager, PlaylistStore, SubtitleSync, Config)   │
└────────────────────────────────────────────────────────┘
```

### Tier 1: Unit Tests (Zero Engine Dependencies)
- `tests/test_queue_manager.cpp`: Validates queue operations (add, remove, reorder, shuffle, repeat modes, auto-advance). Executes in under 15ms.
- `tests/test_playlist_store.cpp`: Validates M3U8 and JSON parsing and writing, handling special characters and path normalization.
- `tests/test_settings_store.cpp`: Validates schema migrations, default fallbacks, and serialization integrity.

### Tier 2: Engine Integration Tests
- `tests/test_libvlc_harness.cpp`: Confirms LibVLC instance creation, media descriptor generation, event listener attachments, and clean instance termination without leaks.

### Tier 3: Playback Validation
- Tests utilize small, redistributable royalty-free test clips (e.g. 5-second Big Buck Bunny H.264/AAC sample) to verify seek accuracy, track enumeration, and audio muting.
