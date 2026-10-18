# 📜 Tomato — Architecture Decision Records (ADRs)

> **Document Version:** 1.0.0  
> **Repository:** `tomato`  
> **Status:** Ratified Records  

---

## Index of Decisions

- [ADR-001: Architecture Choice — Standalone Application vs VLC Monolithic Fork](#adr-001-architecture-choice--standalone-application-vs-vlc-monolithic-fork)
- [ADR-002: Media Engine — LibVLC with Strict Abstract Interface](#adr-002-media-engine--libvlc-with-strict-abstract-interface)
- [ADR-003: UI Framework — Qt 6 / QML with Native C++20 Core](#adr-003-ui-framework--qt-6--qml-with-native-c20-core)
- [ADR-004: Domain Model Separation — Transient Queue vs Persistent Playlist](#adr-004-domain-model-separation--transient-queue-vs-persistent-playlist)
- [ADR-005: Local-First Privacy, Zero Telemetry, and Storage Security](#adr-005-local-first-privacy-zero-telemetry-and-storage-security)
- [ADR-006: Subtitle Workspace Architecture & Phased Remote Providers](#adr-006-subtitle-workspace-architecture--phased-remote-providers)
- [ADR-007: Original Branding, Vector Assets, and Trademark Compliance](#adr-007-original-branding-vector-assets-and-trademark-compliance)

---

## ADR-001: Architecture Choice — Standalone Application vs VLC Monolithic Fork

- **Date:** 2026-10-18  
- **Status:** Accepted  

### Context
VLC Media Player's codebase is a 25-year-old C/C++ repository exceeding 1.5 million lines of code. It uses Autotools and requires compiling 150+ third-party `contrib` dependencies. The current desktop interface (`modules/gui/qt`) is tightly bound to internal VLC player abstractions. The project brief requires evaluating whether to:
1. Rework VLC's existing Qt/QML desktop interface in a personal fork.
2. Build a standalone desktop application with a modern UI and a LibVLC integration layer.

### Decision
We choose **Option 2: Standalone Desktop Application with a LibVLC Integration Layer**.

### Rationale
- **Development Velocity:** Compiling the full VLC repository from source takes hours and requires intricate cross-compilation toolchains. A standalone application builds in seconds using standard modern CMake and C++20.
- **Maintainability:** Modifying `modules/gui/qt` causes endless merge conflicts when rebasing on upstream VideoLAN commits. A standalone app consumes LibVLC as a modular, versioned dependency.
- **UI Freedom:** We are unburdened by legacy QWidget event cascades, allowing us to build a fluid, hardware-accelerated QML desktop shell.
- **Licensing Flexibility:** LibVLC is licensed under LGPL-2.1+, enabling Tomato's application shell to be licensed under MIT or Apache 2.0 while remaining legally compliant.

---

## ADR-002: Media Engine — LibVLC with Strict Abstract Interface

- **Date:** 2026-10-18  
- **Status:** Accepted  

### Context
Tomato requires universal playback of virtually every video, audio, and network stream container and codec without forcing users to install external codec packs. Building a custom decoder or multimedia pipeline from scratch would take years.

### Decision
We adopt **LibVLC (3.0.x / 4.x compatible)** as Tomato's media engine, mediated exclusively through a pure virtual C++ interface (`IPlayerEngine`).

### Rationale
- LibVLC provides battle-tested playback across all major platforms with hardware acceleration (VideoToolbox on macOS, Direct3D11 on Windows, VA-API/VDPAU on Linux).
- By requiring all UI and domain components to communicate only through `IPlayerEngine`, the core application never includes `vlc/vlc.h` directly. This enables:
  - Unit testing with mock engines.
  - Future support for alternative engines (e.g., LibMPV or native OS media foundations) if desired.

---

## ADR-003: UI Framework — Qt 6 / QML with Native C++20 Core

- **Date:** 2026-10-18  
- **Status:** Accepted  

### Context
The user interface must deliver near-black aesthetics (`#080909`), subtle animations, custom SVG icons, a thick timeline with hover previews, and instant responsiveness without high memory overhead or latency.

### Decision
We select **Qt 6 / QML (Qt Quick)** paired with a **C++20** backend core.

### Rationale
- **Performance:** Native compiled C++ execution ensures near-zero startup delay and minimal idle RAM usage (~40-80 MB vs 300+ MB for Electron).
- **GPU Acceleration:** QML's Scene Graph renders overlays, progress fills, and hover effects at native display refresh rates (60Hz / 120Hz ProMotion).
- **Cross-Platform Window Embedding:** Qt Quick provides native integration hooks for platform window handles (`NSView`, `HWND`, `X11/Wayland`).
- Web stacks (Electron / React wrappers) are explicitly rejected to preserve desktop performance, battery efficiency, and local-first architectural simplicity.

---

## ADR-004: Domain Model Separation — Transient Queue vs Persistent Playlist

- **Date:** 2026-10-18  
- **Status:** Accepted  

### Context
A primary UX flaw in legacy media players is the ambiguity between the active playback queue and saved playlists. Modifying the playing list often inadvertently alters saved playlist files on disk.

### Decision
Tomato strictly decouples the domain models into two distinct classes:
1. `QueueManager`: Manages the ephemeral session playback order. Exists in-memory for the current session. Supports drag-and-drop reordering, quick enqueue, skip, and clear.
2. `PlaylistStore`: Manages durable, named collections saved to disk (`.m3u8` or Tomato JSON). Persistent across restarts; modifications are explicit.

### Rationale
Clear domain separation prevents accidental data loss, simplifies undo/redo states, and aligns with user mental models.

---

## ADR-005: Local-First Privacy, Zero Telemetry, and Storage Security

- **Date:** 2026-10-18  
- **Status:** Accepted  

### Context
Modern desktop software frequently introduces invasive telemetry, mandatory logins, and cloud data aggregation.

### Decision
Tomato operates under a **Strict Local-First Architecture**:
- Zero analytics, telemetry, pingbacks, or background tracking.
- All watch progress, library indexes, and preferences are stored exclusively on local disk.
- Network requests occur solely when opening user-specified network streams or when explicitly initiating a remote subtitle search.

---

## ADR-006: Subtitle Workspace Architecture & Phased Remote Providers

- **Date:** 2026-10-18  
- **Status:** Accepted  

### Context
Subtitle synchronization, styling, and track selection are core differentiating features of Tomato. Subtitle handling involves local file parsing, embedded stream switching, timing calibration, and potential remote provider downloads.

### Decision
We partition subtitle functionality into three distinct milestones:
- **Milestone 1 (Local Core):** Embedded track selection, opening local `.srt`/`.ass` files, millisecond delay calibration, and local font/outline styling.
- **Milestone 2 (Local Automation):** Automatic sibling folder matching for TV episodes and per-series style presets.
- **Milestone 3 (Remote Integration):** Optional, user-initiated remote subtitle search via vetted community providers (e.g., OpenSubtitles) with explicit user confirmation before any download.

### Rationale
Isolating remote network dependencies from local playback guarantees that the core subtitle workspace is robust, reliable, and completely functional offline.

---

## ADR-007: Original Branding, Vector Assets, and Trademark Compliance

- **Date:** 2026-10-18  
- **Status:** Accepted  

### Context
VideoLAN holds registered trademarks on the "VLC" name and traffic cone logo. Tomato must establish its own unique identity while properly crediting upstream open-source contributions.

### Decision
- The application identity is **Tomato** with the tagline *"Every format. A better experience."*
- We create original vector SVG logos (`assets/logo/tomato-mark.svg` and `tomato-logo.svg`) and a custom 24x24 icon library.
- The traffic cone is not used as Tomato's brand logo; however, a subtle tapered cone motif is tastefully adopted as a volume slider in homage to VLC's legacy.
- All upstream VideoLAN copyright notices and LibVLC license attributions are preserved in documentation and the application's About panel.
