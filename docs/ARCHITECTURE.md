# 🏛️ Tomato — Architectural Specification & Engine Evaluation

> **Document Version:** 1.0.0  
> **Target Release:** v0.1.0-alpha  
> **Status:** Approved Architectural Baseline  

---

## 1. Executive Evaluation & Decision

A central architectural decision for **Tomato** is choosing between two distinct paths:
- **Option A:** Rework VLC's existing Qt/QML desktop interface inside a fork of VideoLAN's monolithic repository (`modules/gui/qt`).
- **Option B:** Build a standalone, modern Tomato desktop application featuring a dedicated **LibVLC** integration layer and strict component boundaries.

### Comparative Evaluation Matrix

| Criterion | Option A: Fork VLC Desktop Interface | Option B: Standalone Tomato + LibVLC Adapter (Recommended) |
| :--- | :--- | :--- |
| **Time to First Usable Release** | **Slow (Months)**: Modifying VLC's existing C++/Qt4/Qt5 QWidgets & QML architecture requires deep immersion into 1.5M lines of legacy code. | **Fast (Weeks)**: Focuses directly on modern UI/UX and domain logic, consuming a pre-verified, rock-solid LibVLC media engine. |
| **Build System & Tooling** | **Complex & Fragile**: Relies on Autotools (`Makefile.am`), requiring 150+ third-party `contrib` libraries. Compiling on macOS/Windows frequently fails on SDK version shifts. | **Standard Modern CMake**: Fast compilation in seconds using standard modern CMake + C++20 + Qt 6. Zero need to recompile decoders. |
| **UI Innovation & Polish** | **Constrained**: Bound by legacy VLC event cascades, dialog managers, and mixed QWidget/QML legacy patterns. | **Unconstrained**: Clean QML / Qt Quick 6 canvas, GPU-accelerated micro-animations, custom SVG icon system, pixel-perfect dark theme. |
| **Maintainability & Upstream Sync** | **High Burden**: Merging upstream VLC changes into a heavily patched `modules/gui/qt` causes severe, recurring merge conflicts. | **Low Burden**: Upstream VLC updates are consumed cleanly as updated LibVLC binaries/dylibs. Zero patch friction. |
| **Playback & Codec Compatibility** | **Native**: Identical (VLC core). | **Native**: Identical (Uses identical LibVLC core, codecs, demuxers, hardware acceleration, and network protocols). |
| **API Expressiveness** | **Direct**: Can reach internal VLC C structs directly. | **Comprehensive**: LibVLC exposes 100% of required player APIs: track switching, subtitle offset, aspect ratio, audio filters, volume, seeking. |
| **Domain Model Separation** | **Mixed**: VLC historically entangles playlist with active playback queue and media library. | **Strict**: Clean separation of `QueueManager` (ephemeral) and `PlaylistStore` (persistent) in testable C++ models. |
| **Licensing & Redistribution** | **Strict GPL-2.0+**: Entire fork must be licensed under GPL. | **Modular LGPL-2.1+**: LibVLC is LGPL-2.1. The Tomato application shell can be licensed under permissive MIT/Apache-2.0 or GPL. |
| **Testing & Contributor Onboarding** | **High Barrier**: New contributors must install complex cross-compilation toolchains. Running tests requires the full media engine. | **Low Barrier**: Standard C++ tests run without a media pipeline. Clear abstractions allow mocking player events. |

### Architectural Verdict
**Option B is strongly recommended and adopted.**  
By building a standalone desktop shell around the official LibVLC engine, Tomato eliminates months of build infrastructure overhead while delivering complete visual and interaction freedom.

---

## 2. Upstream VLC Architecture Audit

VLC Media Player's core codebase is organized as follows:
- `include/`: Public LibVLC API declarations (`vlc/vlc.h`, `vlc/libvlc_media_player.h`, etc.).
- `lib/`: Core implementation of the public LibVLC C API (`media_player.c`, `playlist.c`, `audio.c`, `video.c`).
- `src/`: The internal multimedia runtime (`libvlccore`), managing module loading, thread scheduling, clock synchronization, input demuxers, and output pipelines.
- `modules/`: Hundreds of modular plugins (demuxers, audio/video filters, hardware decoders like VideoToolbox/VAAPI/Direct3D11, network access modules, codecs).
- `modules/gui/qt/`: The traditional Qt GUI. Historically built with QWidgets, transitioning incrementally to QML in VLC 4.x. It relies heavily on internal non-public headers (`vlc_common.h`, `vlc_player.h`).

### Verified Local Environment Findings
Empirical verification on macOS Darwin arm64 confirms:
- **LibVLC SDK Location:** `/Applications/VLC.app/Contents/MacOS/lib/libvlc.dylib` (version 3.0.20, Mach-O 64-bit arm64).
- **Public Headers:** `/Applications/VLC.app/Contents/MacOS/include/vlc/` containing all required headers (`libvlc.h`, `libvlc_media_player.h`, `libvlc_events.h`).
- **Plugins Directory:** `/Applications/VLC.app/Contents/MacOS/plugins/` (343 active plugins).
- **Clang Link Verification:** Successfully compiled and executed a native C test harness verifying instance creation, event registration, and safe release.

---

## 3. Tomato Target Architecture

Tomato employs a **Strict Layered Architecture** with unidirectional dependency flows:

```
┌────────────────────────────────────────────────────────────────────────┐
│                        PRESENTATION LAYER (QML)                        │
│                                                                        │
│  ┌─────────────────────────┐  ┌─────────────────────────────────────┐  │
│  │ TopBar / WindowControls │  │ VideoSurface (Black Canvas #000000) │  │
│  └─────────────────────────┘  └─────────────────────────────────────┘  │
│  ┌─────────────────────────┐  ┌─────────────────────────────────────┐  │
│  │ ContextMenu & Drawers   │  │ ControlBar (Timeline, Tapered Vol)  │  │
│  └─────────────────────────┘  └─────────────────────────────────────┘  │
└───────────────────────────────────┬────────────────────────────────────┘
                                    │ Property Bindings & Signals
┌───────────────────────────────────▼────────────────────────────────────┐
│                    APPLICATION CORE LAYER (C++20)                      │
│                                                                        │
│  ┌─────────────────────────┐  ┌─────────────────────────────────────┐  │
│  │   PlaybackController    │  │            QueueManager             │  │
│  │ (State, Time, Position) │  │    (Transient Session Playback)     │  │
│  └────────────┬────────────┘  └──────────────────┬──────────────────┘  │
│               │                                  │                     │
│  ┌────────────▼────────────┐  ┌──────────────────▼──────────────────┐  │
│  │   SubtitleSyncEngine    │  │            PlaylistStore            │  │
│  │ (Offset, Style, Presets)│  │     (Persistent Saved Lists)        │  │
│  └────────────┬────────────┘  └──────────────────┬──────────────────┘  │
│               │                                  │                     │
│  ┌────────────▼────────────┐  ┌──────────────────▼──────────────────┐  │
│  │      SettingsStore      │  │          MediaLibraryIndex          │  │
│  │   (JSON/SQLite Config)  │  │      (Folder Scanner & Search)      │  │
│  └─────────────────────────┘  └─────────────────────────────────────┘  │
└───────────────────────────────────┬────────────────────────────────────┘
                                    │ Pure Abstract Interfaces
┌───────────────────────────────────▼────────────────────────────────────┐
│                   MEDIA ENGINE BOUNDARY (C++20)                        │
│                                                                        │
│                       <<interface>> IPlayerEngine                      │
│   + loadMedia(uri)                 + getPosition() / setPosition(pos)  │
│   + play() / pause() / stop()      + getVolume() / setVolume(vol)      │
│   + getAudioTracks() / select()    + getSubtitleTracks() / select()    │
│   + setSubtitleDelay(ms)           + setAudioDelay(ms)                 │
│   + attachVideoWindow(windowId)    + addObserver(IPlayerObserver*)     │
└───────────────────────────────────┬────────────────────────────────────┘
                                    │ Adapter Implementation
┌───────────────────────────────────▼────────────────────────────────────┐
│                   LIBVLC ADAPTER (engine/libvlc-adapter)               │
│                                                                        │
│  - Wraps libvlc_instance_t and libvlc_media_player_t                  │
│  - Attaches to native window handles (NSView, HWND, X11/Wayland)       │
│  - Subscribes to libvlc_event_manager_t signals                        │
│  - Dispatches thread-safe asynchronous events to Application Core     │
└────────────────────────────────────────────────────────────────────────┘
```

---

## 4. Module Decomposition

### 1. `app/` (Presentation Layer)
- **`app/shell/`**: Root window management, frameless dark window decoration, fullscreen toggling, high-DPI scaling, cursor auto-hiding.
- **`app/player/`**: The primary playback viewport, video letterboxing, thick timeline scrubber, playback button row, and tapered volume slider.
- **`app/queue/`**: Drag-and-drop transient queue drawer, reorder animations, remove, skip, and enqueue actions.
- **`app/subtitles/`**: The Subtitle Workspace panel (visual delay slider with millisecond readout, track picker, real-time font preview).
- **`app/library/`**: Local media browser, folder view, search bar, recent files grid/list, watched badges.
- **`app/settings/`**: Searchable preference panels, keyboard shortcuts editor, theme and accent customizer.

### 2. `core/` (Application Domain Logic)
- **`core/playback/playback_controller.{h,cpp}`**: Mediates between UI requests and the player engine. Holds current play state, time, duration, mute, and speed.
- **`core/playback/queue_manager.{h,cpp}`**: Domain model for the current playback session. Manages queue items, shuffle, loop modes (Off, Repeat One, Repeat All), and auto-advance.
- **`core/playback/playlist_store.{h,cpp}`**: Durable playlist manager. Reads/writes `.m3u8` and Tomato JSON playlists. Independent from the active queue.
- **`core/preferences/settings_store.{h,cpp}`**: Strongly typed configuration storage. Manages defaults, migrations, and serialization to user data directories.
- **`core/media/media_library_index.{h,cpp}`**: Asynchronous filesystem scanner for specified media folders. Extracts basic tags (title, artist, duration) without blocking the UI.

### 3. `engine/` (Media Engine Interface & Adapter)
- **`engine/player-interface/player_interface.h`**: Abstract C++ contract. Pure virtual interface ensuring the application core never includes `vlc/vlc.h` directly.
- **`engine/libvlc-adapter/libvlc_adapter.{h,cpp}`**: Concrete implementation wrapping `libvlc`. Translates engine calls, handles video window attachment, and manages LibVLC event loops.

### 4. `assets/` (Design Assets)
- **`assets/logo/`**: Vector wordmark and tomato icon.
- **`assets/icons/`**: 24x24 custom SVG icons with consistent stroke geometry, rounded line caps/joins, and CSS/QML color variables.
- **`assets/themes/`**: Token declarations for the dark aesthetic.

---

## 5. Concurrency & Threading Model

```
       UI Thread (Main)                   LibVLC Engine Threads
  ┌─────────────────────────┐           ┌────────────────────────┐
  │ QML Rendering Loop      │           │ Demuxing & Decoding    │
  │ User Gestures (Seek)    │           │ Video Output (VOUT)    │
  │ Animation Timelines     │           │ Audio Output (AOUT)    │
  └───────────┬─────────────┘           └───────────┬────────────┘
              │ Direct Method Call                  │
              ▼                                     ▼
  ┌─────────────────────────┐           ┌────────────────────────┐
  │   PlaybackController    │           │ libvlc_event_manager_t │
  │   - setPosition(0.42)   │           │ - MediaPlayerPosition  │
  │   - setVolume(85)       │           │ - MediaPlayerTime      │
  └───────────┬─────────────┘           └───────────┬────────────┘
              │ Thread-Safe Invocation              │
              ▼                                     │ LibVLC Callback
  ┌─────────────────────────┐                       │ (Engine Thread)
  │      LibVLCAdapter      │◄──────────────────────┘
  │ - libvlc_media_player_* │
  └───────────┬─────────────┘
              │ Post Event to Qt Main Loop (QCoreApplication::postEvent)
              ▼
  ┌─────────────────────────┐
  │ UI Notification Update  │ (Smooth, decoupled 60fps refresh)
  └─────────────────────────┘
```

1. **Main UI Thread:** Owns Qt Quick rendering and immediate user feedback. Scrubbing a timeline updates the visual playhead instantly without waiting for the engine seek to complete.
2. **LibVLC Engine Thread:** LibVLC operates its own internal threads for demuxing, decoding, and audio/video output. Engine event callbacks run on LibVLC internal threads.
3. **Synchronization Barrier:** The `LibVLCAdapter` captures LibVLC callbacks and marshals them back to the Main UI Thread via thread-safe signals/events (`QMetaObject::invokeMethod` with `Qt::QueuedConnection`).
4. **I/O Worker Pool:** Heavy tasks like scanning local media folders, reading ID3 tags, and parsing subtitle files are dispatched to background worker threads.

---

## 6. Video Surface Embedding Strategy

To achieve zero-copy, hardware-accelerated video rendering:
- **macOS:** LibVLC renders directly into a native Cocoa `NSView` layer using `libvlc_media_player_set_nsobject()`.
- **Windows:** LibVLC renders into a native Win32 `HWND` via `libvlc_media_player_set_hwnd()`.
- **Linux:** LibVLC renders into an X11 `Drawable` or Wayland surface via `libvlc_media_player_set_xwindow()`.
- **Fallback / Off-screen:** If custom QML shader effects are required over the video, LibVLC's memory video output callbacks (`libvlc_video_set_callbacks` and `libvlc_video_set_format_callbacks`) can pipe raw decoded frames into an OpenGL/Metal texture. For maximum battery life and performance, native window surface embedding is preferred for primary playback.
