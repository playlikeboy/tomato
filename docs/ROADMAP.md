# 🗺️ Tomato — Implementation Roadmap & Milestones

> **Document Version:** 1.0.0  
> **Target Release:** v0.1.0-alpha to v1.0.0  
> **Status:** Active Execution Plan  

---

## 1. Phased Development Overview

Tomato is developed across seven rigorous, sequential phases. Each phase establishes a verified, testable baseline before subsequent layers are built.

```
Phase 0 ──► Phase 1 ──► Phase 2 ──► Phase 3 ──► Phase 4 ──► Phase 5 ──► Phase 6
Audit &     Design &    Core Media  Queue &     Subtitle &  Settings &  Packaging &
Feasibility App Shell   Playback    Playlists   Audio Hub   A11y/Polish Distribution
```

---

## Phase 0: Repository Audit & Build Feasibility (CURRENT)

**Objective:** Audit developer toolchains, inspect LibVLC integration capabilities on native operating systems, establish architectural boundaries, and verify compile/link pathways.

### Concrete Tasks
- [x] Audit local environment: Clang/C++20, CMake, Darwin arm64 architecture, Homebrew libraries.
- [x] Verify LibVLC binary compatibility: Discover headers (`include/vlc`) and shared dylibs (`libvlc.dylib`, `libvlccore.dylib`) inside `/Applications/VLC.app`.
- [x] Compile and execute native LibVLC verification harness (`tests/test_libvlc_harness`).
- [x] Formalize architectural specification: Evaluate Option A (VLC Fork) vs Option B (Standalone Desktop Shell).
- [x] Draft core planning deliverables (`VISION.md`, `ARCHITECTURE.md`, `UI_UX_SPEC.md`, `ROADMAP.md`, `DECISIONS.md`, `SECURITY_AND_PRIVACY.md`, `BUILD_AND_TESTING.md`).
- [x] Establish Git repository, branch strategy (`main`), and initial commit structure.

### Acceptance Criteria
- Native C/C++ test program links dynamically with LibVLC, initializes an engine instance, queries runtime version (`3.0.20 Vetinari`), and cleanly frees resources with zero memory faults.
- All core planning documents approved and cross-referenced.

---

## Phase 1: Brand, Design System & Application Shell

**Objective:** Implement the visual foundation, brand marks, custom vector icons, and an interactive desktop window shell adhering to the `#080909` dark aesthetic.

### Concrete Tasks
- [ ] Implement vector brand assets: `assets/logo/tomato-logo.svg` and `assets/logo/tomato-mark.svg`.
- [ ] Build the complete 24x24 SVG icon set:
  - Toolbar monochrome: `play`, `pause`, `stop`, `previous`, `next`, `volume-high`, `volume-low`, `volume-mute`, `subtitles`, `audio-track`, `queue`, `playlist`, `settings`, `fullscreen`, `pip`, `record`.
  - Context menu two-tone: `menu-playback`, `menu-video`, `menu-audio`, `menu-subtitle`, `menu-settings`, `menu-playlist`, `menu-open`.
- [ ] Build desktop application window shell with frameless dark chrome, custom title bar, and native window control bridges.
- [ ] Implement responsive QML layout regions:
  - Header bar with title display.
  - Black media canvas area (`#000000`).
  - Elevated bottom control bar (`#191919`).
  - Context menu component with subtle entry transitions and keyboard navigation.
- [ ] Implement the tapered cone-inspired volume widget geometry.

### Dependencies
- Phase 0 verification complete; CMake build setup for QML and C++ application shell.

### Deferred
- Live video decoding into the canvas (Phase 2).
- Network streaming input fields (Phase 6).

---

## Phase 2: Core Media Playback & Essential Controls

**Objective:** Connect the application shell to the LibVLC engine adapter for rock-solid local video and audio playback.

### Concrete Tasks
- [ ] Implement `IPlayerEngine` abstract C++ interface in `engine/player-interface/player_interface.h`.
- [ ] Implement `LibVLCAdapter` in `engine/libvlc-adapter/` wrapping `libvlc_media_player_t`.
- [ ] Connect native video rendering surface:
  - macOS: Embed Cocoa `NSView` using `libvlc_media_player_set_nsobject()`.
  - Windows: Embed Win32 `HWND` using `libvlc_media_player_set_hwnd()`.
  - Linux: Embed X11 `Drawable` or Wayland surface.
- [ ] Implement core playback controls: Open file, Play, Pause, Stop, Seek, Step forward/backward.
- [ ] Implement thick timeline scrubber with real-time position updates, drag interaction, and timestamp displays (`hh:mm:ss`).
- [ ] Connect audio volume and mute controls to LibVLC audio APIs.
- [ ] Implement keyboard shortcut dispatcher (`Space`, `F`, Arrow keys, `M`, `S`).
- [ ] Implement auto-hiding controls during fullscreen playback with smooth fade transitions on pointer activity.

### Acceptance Criteria
- Opens local MP4, MKV, AVI, and WebM video files without hitching or frame drop.
- Scrubbing the seek bar responds instantly; time readouts stay in perfect sync.
- Zero crashes on rapid open, seek, and close cycles.

---

## Phase 3: Queue, Playlists & Local Media Basics

**Objective:** Deliver clean, decoupled domain models for the transient playback queue, persistent playlists, and local folder browsing.

### Concrete Tasks
- [ ] Implement `core/playback/queue_manager`:
  - Ephemeral session queue.
  - Operations: Enqueue, Prepend, Remove, Clear, Move/Reorder.
  - Playback modes: Sequential, Shuffle, Repeat None, Repeat One, Repeat All.
  - Automatic transition to next queue item on media end.
- [ ] Implement `core/playback/playlist_store`:
  - Durable, named collections.
  - Serialization to standard `.m3u8` playlists and structured Tomato JSON.
  - Save, load, rename, and export capabilities.
- [ ] Build the interactive Queue Drawer UI:
  - Slide-out right panel.
  - Drag-and-drop reordering with live list animations.
  - Visual indicator for currently active track.
- [ ] Drag-and-drop file ingestion:
  - Dropping video onto canvas immediately starts playback.
  - Dropping multiple files adds them to the active queue.
  - Dropping a directory offers to enqueue or browse.

### Acceptance Criteria
- Unit tests pass for Queue reordering, boundary indices, shuffle integrity, and repeat transitions.
- Playlists persist across application restarts; modifying the queue never corrupts saved playlists.

---

## Phase 4: Subtitle & Audio Track Workspace

**Objective:** Build Tomato's signature subtitle synchronization and multi-track audio experience.

### Milestone 4.1: Embedded & Local Track Selection
- [ ] Enumerate embedded subtitle and audio tracks via LibVLC track APIs.
- [ ] Expose instant track switcher popovers on the control bar.
- [ ] Add "Load External Subtitle..." file dialog supporting `.srt`, `.ass`, `.vtt`, `.sub`.

### Milestone 4.2: Visual Subtitle Synchronization
- [ ] Implement Subtitle Delay Calibration Slider:
  - Range `-5000 ms` to `+5000 ms` with millisecond readout.
  - Step buttons (`-50 ms`, `+50 ms`) with immediate audio/video re-sync.
- [ ] Keyboard shortcuts for quick offset adjustment (`G` / `H`).
- [ ] Real-time subtitle formatting: font family, font size scaling, text color, outline width, and vertical position offset.

### Milestone 4.3: Smart Local Matching & Presets
- [ ] Automatic sibling directory search for matching subtitle files (`<filename>.srt`, `<filename>.en.srt`).
- [ ] Per-series subtitle styling presets saved locally.

### Deferred to Milestone 4.4+
- Remote subtitle provider search (OpenSubtitles REST API) with explicit user consent.
- Audio waveform visualizer.

---

## Phase 5: Settings, Customization & Accessibility

**Objective:** Provide organized, searchable preferences, custom keyboard shortcuts, and verified WCAG 2.1 AA accessibility.

### Concrete Tasks
- [ ] Implement `core/preferences/settings_store` with JSON persistence in OS configuration folders (`~/Library/Application Support/Tomato/` on macOS, `%APPDATA%/Tomato/` on Windows).
- [ ] Build search-indexed Settings window:
  - General, Appearance, Playback, Subtitles, Audio, Video, Shortcuts, Privacy.
  - Progressive disclosure: "Simple" mode by default, toggleable "Advanced" options.
- [ ] Keyboard shortcuts customization editor: Remap keys with conflict detection.
- [ ] Accessibility audit:
  - Verify WCAG 2.1 AA contrast across all themes.
  - Implement full keyboard tab-navigation and focus traps in dialogs.
  - Screen reader announcements for state changes (e.g., "Paused at 12 minutes 40 seconds").

---

## Phase 6: Network Streams, Packaging & Release Hardening

**Objective:** Add network stream playback, build CI pipelines, package native binaries, and prepare for public open-source release.

### Concrete Tasks
- [ ] Network stream ingestion: Open HTTP/HTTPS, HLS, RTSP, RTMP URLs.
- [ ] Build automated CI workflows (GitHub Actions) for macOS (arm64/x86_64), Linux (AppImage/Flatpak), and Windows (MSI/NSIS).
- [ ] Package standalone macOS `.dmg` and `.app` bundle including LibVLC dylibs and notarization.
- [ ] Memory leak audit: Run Valgrind / macOS Leaks instrument on 24-hour continuous playback loops.
- [ ] Prepare open-source launch documentation, release notes, and contribution guidelines.
