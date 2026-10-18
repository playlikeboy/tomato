<div align="center">

# 🍅 Tomato

**Every format. A better experience.**

*A modern, beautifully designed, open-source desktop media player powered by LibVLC.*

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg?style=flat-square)](./LICENSE)
[![Engine: LibVLC 3.0+](https://img.shields.io/badge/Engine-LibVLC%203.0%2B-orange.svg?style=flat-square)](https://www.videolan.org/vlc/libvlc.html)
[![Standard: C++20](https://img.shields.io/badge/Standard-C%2B%2B20-blue.svg?style=flat-square)](#)
[![Build: CMake](https://img.shields.io/badge/Build-CMake%203.22%2B-brightgreen.svg?style=flat-square)](#)
[![Platform: macOS | Linux | Windows](https://img.shields.io/badge/Platform-macOS%20%7C%20Linux%20%7C%20Windows-lightgrey.svg?style=flat-square)](#)

<p align="center">
  <a href="#-the-visual-direction"><strong>Visual Direction</strong></a> &bull;
  <a href="#-why-tomato"><strong>Why Tomato?</strong></a> &bull;
  <a href="#-architecture"><strong>Architecture</strong></a> &bull;
  <a href="#-roadmap"><strong>Roadmap</strong></a> &bull;
  <a href="#-getting-started"><strong>Getting Started</strong></a> &bull;
  <a href="./docs/"><strong>Documentation</strong></a>
</p>

</div>

---

## 📽️ The Visual Direction

Tomato's interface is designed with restraint. The UI never fights the content.

```text
┌──────────────────────────────────────────────────────────────────────────────┐
│ 🍅  The Wand S01E04                                              —  □  ✕     │
├──────────────────────────────────────────────────────────────────────────────┤
│                                                                              │
│                                                                              │
│                             VIDEO CANVAS (#000000)                           │
│                                                                              │
│                                ┌───────────────────────────┐                 │
│                                │ ▷ Play              Space │                 │
│                                │ □ Stop                  S │ Context Menu    │
│                                │ ◁ Previous              P │ (Right Click)   │
│                                │ ▷ Next                  N │                 │
│                                │ ◉ Record                R │                 │
│                                │ ───────────────────────── │                 │
│                                │ ▷ Playback              > │                 │
│                                │ ▤ Subtitle              > │                 │
│                                │ ⚙ More Settings         > │                 │
│                                │ ✕ Quit Tomato      Ctrl+Q │                 │
│                                └───────────────────────────┘                 │
│                                                                              │
├──────────────────────────────────────────────────────────────────────────────┤
│ ──[===========================○───────────────────────────────────────────]─ │
│                                                                              │
│  [◁] [ ▷ ] [▷]  00:00:02                               00:25:40  ◁ [ ▂▃▅█] ▤ ♬ ☰ ⛶ ⚙ │
└──────────────────────────────────────────────────────────────────────────────┘
```

### Design System Tokens
- **Canvas:** `#000000` true black letterboxing.
- **Surfaces:** `#080909` (primary window background) & `#191919` (elevated control bar and floating panels).
- **Primary Accent:** `#E96914` (Tomato Orange) for active playback buttons, timeline progress, and state indicators.
- **Tapered Volume Slider:** A cone-inspired graduated slider paying homage to VLC’s heritage.
- **Custom Vector Assets:** Shared `0 0 24 24` SVG icons with monochrome toolbar styling and two-tone menu artwork.

---

## 💡 Why Tomato?

For over 25 years, **VLC Media Player** has been the undisputed gold standard for codec compatibility and format playback. However, its traditional desktop interface has grown dense and dated. Ordinary tasks—such as synchronizing an out-of-sync subtitle track, building a temporary playback queue, or discovering audio streams—require hunting through deep menus.

**Tomato preserves VLC's core superpower while solving its desktop UX:**

| Feature | Legacy Experience | Tomato Experience |
| :--- | :--- | :--- |
| **Visual Aesthetics** | Dated gray dialogs and inconsistent controls | Restrained near-black `#080909` theme with tomato-orange accents |
| **Subtitle Calibration** | Trial-and-error typing in millisecond offsets | Visual **Subtitle Workspace** with delay slider and live typography preview |
| **Queue vs Playlist** | Conflated together; modifying queue alters files | **Strict separation**: Ephemeral session queue vs. durable saved playlists |
| **Seek Bar** | Narrow, difficult to target accurately | Thick, responsive timeline with hover timestamp preview |
| **Volume Control** | Standard linear slider | Tapered cone-inspired slider with clear boost threshold |
| **Privacy & Sovereignty** | Varied across modern players | **100% Local-first**: Zero telemetry, zero analytics, offline-first |

---

## 🏛️ Architecture

Tomato decouples the user interface from the underlying media engine using a **Strict Layered Boundary**:

```
┌────────────────────────────────────────────────────────┐
│                   TOMATO DESKTOP APP                   │
├────────────────────────────────────────────────────────┤
│   Presentation Layer: Qt 6 / QML Shell & Custom SVG    │
│   (#080909 Dark Theme, Two-Tone Icons, Fluid Layouts)  │
├────────────────────────────────────────────────────────┤
│   Application Core: C++20 Domain Logic                 │
│   - QueueManager       - SubtitleSyncEngine            │
│   - PlaylistStore      - Local Media Indexer           │
│   - SettingsStore      - Shortcut Dispatcher           │
├────────────────────────────────────────────────────────┤
│   Media Engine Interface: IPlayerEngine (C++20)        │
├────────────────────────────────────────────────────────┤
│   LibVLC Adapter: (libvlc.dylib / libvlc.so / libvlc)  │
│   - Universal Decoders - Hardware Acceleration (Metal) │
└────────────────────────────────────────────────────────┘
```

See [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) and [docs/DECISIONS.md](docs/DECISIONS.md) for complete technical specifications.

---

## 🚀 Getting Started

### Prerequisites
- Modern C++20 compiler (Apple Clang 15+, GCC 12+, MSVC 2022+)
- CMake 3.22+
- LibVLC 3.0.x development headers and runtime (for example, `/Applications/VLC.app` on macOS or `libvlc-dev` plus VLC plugins on Linux)
- Qt 6 Widgets 6.2+ for the native desktop application

### Environment Validation
Run our environment audit script to verify toolchains and LibVLC discovery:
```bash
./scripts/check_env.sh
```

### Build, Test & Run the Native Desktop Player

The native app requires both LibVLC and Qt 6 Widgets. To fail early instead of silently skipping the desktop target:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug \\
  -DTOMATO_REQUIRE_LIBVLC=ON \\
  -DTOMATO_REQUIRE_DESKTOP_APP=ON
cmake --build build --parallel 2
ctest --test-dir build --output-on-failure
./build/tomato
# Or open a file directly:
./build/tomato /path/to/video.mkv
```

On macOS with the VLC application bundle, the runtime plug-in path may need to be set before launch:

```bash
export VLC_PLUGIN_PATH=/Applications/VLC.app/Contents/MacOS/plugins
./build/tomato
```

Use `./build/tomato --version` for a quick executable smoke test. The older HTML file under `app/shell/` remains a visual prototype; the native player lives under `app/desktop/`.

---

## 📚 Project Documentation

- [VISION.md](docs/VISION.md) — Product vision, principles, target personas, and non-goals.
- [ARCHITECTURE.md](docs/ARCHITECTURE.md) — Engine evaluation, component boundaries, threading model, and surface embedding.
- [UI_UX_SPEC.md](docs/UI_UX_SPEC.md) — Color tokens, typography, custom SVG icon system, keyboard shortcuts, and A11y.
- [ROADMAP.md](docs/ROADMAP.md) — Phased release milestones, acceptance criteria, and deferred items.
- [DECISIONS.md](docs/DECISIONS.md) — Architecture Decision Records (ADRs 001–007).
- [SECURITY_AND_PRIVACY.md](docs/SECURITY_AND_PRIVACY.md) — Filesystem boundaries, local storage paths, and zero-telemetry guarantee.
- [BUILD_AND_TESTING.md](docs/BUILD_AND_TESTING.md) — Build prerequisites, commands, CI, and test hierarchy.

---

## ⚖️ Trademarks & Attribution

Tomato is an independent open-source project.
- **LibVLC** is licensed under **LGPL-2.1-or-later** by the VideoLAN team.
- "VLC" and the traffic cone logo are registered trademarks of **VideoLAN**. Tomato is not affiliated with, sponsored by, or endorsed by VideoLAN.
