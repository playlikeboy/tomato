# Tomato Build and Testing Guide

**Status:** build instructions and CI expectations. Environment-specific results must be verified by running the commands below.

## Requirements

- CMake 3.22+
- C++20 compiler
- LibVLC 3.x development headers, runtime and plug-ins
- Qt 6 Widgets 6.2+ for the native desktop app
- Git

The current repository is a standalone Tomato application that uses LibVLC; it does not vendor the complete VLC source tree.

## Linux (Ubuntu/Debian)

```bash
sudo apt-get update
sudo apt-get install --yes cmake g++ pkg-config libvlc-dev vlc-plugin-base qt6-base-dev
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug \
  -DTOMATO_REQUIRE_LIBVLC=ON \
  -DTOMATO_REQUIRE_DESKTOP_APP=ON
cmake --build build --parallel 2
ctest --test-dir build --output-on-failure
./build/tomato --version
./build/tomato /path/to/video.mkv
```

The Qt desktop executable requires a graphical session for normal launch. The `--version` check exits before opening a window, so it is suitable for headless CI.

## macOS

Install Xcode Command Line Tools, CMake, Qt 6 and a compatible LibVLC SDK/runtime. Depending on where the SDK is installed, pass its paths explicitly:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug \
  -DTOMATO_REQUIRE_LIBVLC=ON \
  -DTOMATO_REQUIRE_DESKTOP_APP=ON \
  -DLIBVLC_INCLUDE_DIR=/Applications/VLC.app/Contents/MacOS/include \
  -DLIBVLC_LIBRARY=/Applications/VLC.app/Contents/MacOS/lib/libvlc.dylib
cmake --build build --parallel 2
ctest --test-dir build --output-on-failure
export VLC_PLUGIN_PATH=/Applications/VLC.app/Contents/MacOS/plugins
./build/tomato
```

If your Qt SDK isn't in CMake's default search paths, set `CMAKE_PREFIX_PATH` to the Qt installation prefix. Confirm that the Qt, LibVLC library, and LibVLC plugins share a compatible architecture.

## Windows

Install Visual Studio 2022 C++ build tools, CMake, Qt 6 Widgets, and a matching LibVLC SDK. Set `LIBVLC_SDK_PATH` to the SDK directory before configuring.

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 `
  -DTOMATO_REQUIRE_LIBVLC=ON `
  -DTOMATO_REQUIRE_DESKTOP_APP=ON `
  -DLIBVLC_SDK_PATH="C:/path/to/libvlc-sdk"
cmake --build build --config Debug
ctest --test-dir build -C Debug --output-on-failure
```

Windows packaging and runtime DLL/plugin discovery still require platform-specific verification before a release can claim support.

## CI

The GitHub Actions workflow installs the Linux development dependencies, configures with both required flags enabled, compiles the native desktop target and executes CTest. A missing engine or Qt dependency should fail configuration rather than silently reporting a core-only build.

See [the Tomato CI workflow](../.github/workflows/ci.yml).

## Test tiers

- **Queue unit tests:** queue operations, repeat modes, shuffle navigation, mutations, and observer reentrancy.
- **LibVLC lifecycle smoke test:** confirms the runtime can create and release a LibVLC instance and media player. It does not prove actual audio/video playback.
- **Desktop smoke test:** `tomato --version` confirms the executable starts without creating a GUI window.
- **Manual playback validation:** open a known local media file; verify video/audio, pause/resume, seek, track selection, external subtitle attachment, queue navigation, fullscreen and clean shutdown.

Use small, redistributable fixtures for automated media tests. Do not mark playback as verified until an actual media fixture is played on the target platform.

## Reporting

For each build or test run, record the command, platform, dependency versions, exit code and relevant output. Distinguish **passed**, **failed**, **blocked by environment**, and **not run**. Never infer a passing build solely from source inspection.
