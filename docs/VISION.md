# 🍅 Tomato — Product Vision & Philosophy

> **Product Name:** Tomato  
> **Tagline:** *Every format. A better experience.*  
> **Classification:** Modern, Open-Source Desktop Media Player & Library  
> **License Target:** Open-Source (Application Layer: MIT / Apache-2.0; Engine Integration: LGPL-2.1 compatible)  

---

## 1. Executive Summary

For more than a quarter of a century, **VLC Media Player** has been the undisputed champion of multimedia versatility. Its legendary engine plays virtually every codec, container, network stream, and disc format ever devised without requiring external codec packs.

However, VLC’s user interface was forged in an era of dense desktop dialogs and utility-first design. Today, everyday actions—such as synchronizing misaligned subtitles, managing an active playback queue, organizing a personal media library, or discovering audio tracks—require navigating deeply nested menus, esoteric modal windows, or memorizing obscure hotkeys.

**Tomato** preserves VLC's greatest superpower—**playing almost anything with zero friction**—while re-architecting the desktop experience from the ground up:
1. **Restrained Visual Aesthetics:** A near-black, content-first canvas (`#080909`) with warm tomato-orange accents (`#E96914`), custom two-tone icons, and controls that never fight the video.
2. **Contextual Discoverability:** Bringing routine actions into immediate view without burying power features under multi-level cascades.
3. **First-Class Subtitle Workspace:** Transforming subtitle discovery, delay adjustment, styling, and episode matching into a modern, visual tool.
4. **Distinct Queue & Playlist Models:** Eliminating decades of confusion between temporary playback queues and persistent collections.
5. **Local-First & Privacy-Respecting:** Zero telemetry, no required accounts, and complete offline sovereignty over your personal media.

---

## 2. Core Product Principles

### I. The Content Is the Hero
The media player is a frame for art, not a billboard for itself. Tomato stays out of the way:
- Near-black surfaces (`#080909` background, `#000000` canvas) ensure maximum contrast and zero light-bleed during cinematic viewing.
- Overlays appear on deliberate user pointer movement and vanish smoothly after an idle timeout.
- Hit areas are compact, rectangular, and neatly rounded (`6–10px`), avoiding oversized, distracting controls.

### II. Discoverability Without Simplification
Simplicity does not mean stripping away power. VLC users love capabilities like A/B looping, audio delay calibration, aspect ratio overriding, and hardware acceleration switches. Tomato keeps these capabilities within easy reach:
- Frequent actions are 1 click away on the primary control row.
- Deep controls are accessible via an organized right-click context menu and an integrated contextual drawer.
- Searchable preferences with progressive disclosure replace dense multi-tab preference labyrinths.

### III. Reliable Media Engine Separation
Tomato does not attempt to reinvent 25 years of codec parsing and demuxing from scratch. Instead, it defines a clean, modular abstraction boundary around **LibVLC**, the rock-solid, multi-platform media engine maintained by VideoLAN. This ensures rock-solid stability and universal format playback while decoupling UI innovation from C-level demuxers.

### IV. True Queue vs. Playlist Distinction
One of the most persistent complaints in existing desktop media players is the conflation of the active queue and saved playlists:
- **Playback Queue:** Ephemeral session state. What plays next right now. Easily manipulated via drag-and-drop, quick prepend/append, skip, and clear.
- **Playlists:** Durable, named, user-curated collections saved to disk (`.m3u8`, JSON) that persist across restarts and can be exported or organized into folders.

### V. Subtitles as a First-Class Feature
Subtitles are not an afterthought or a hidden submenu. For billions of international viewers, anime enthusiasts, and accessibility-reliant users, subtitles are essential. Tomato introduces an integrated **Subtitle Workspace** allowing immediate delay adjustment, real-time font and outline styling, and batch episode matching.

### VI. Absolute Privacy & Local Sovereignty
- **No telemetry:** Tomato contains zero tracking beacons, analytics scripts, or user fingerprinting.
- **Local-first metadata:** Media library indexing and watch progress are stored strictly in local SQLite/JSON caches on the user's machine.
- **Opt-in networking:** External metadata fetching (e.g., subtitle providers or album art) requires explicit user authorization and respects metered connections.

---

## 3. Target Audience Personas

### 1. The Cinephile & Media Collector (Marcus, 34)
- **Profile:** Stores high-bitrate 4K HDR rips, MKVs with multiple audio streams (lossless TrueHD, commentary tracks), and external ASS/SRT subtitle files on local NAS storage.
- **Frustrations with VLC:** Subtitle timing offset is trial-and-error; selecting the right audio track requires digging through menus; UI looks jarring on an OLED monitor.
- **Why Tomato wins:** Elegant dark aesthetics, instant subtitle delay scrubber with millisecond precision, seamless multi-audio track switcher, and HDR tone-mapping visibility.

### 2. The Binge-Watcher & Anime Enthusiast (Aoi, 22)
- **Profile:** Watches episodic TV shows, anime seasons with stylised soft subs, and downloaded web video batches.
- **Frustrations with VLC:** Dragging an entire season folder clutters the playlist; no clear "Next Episode" prompt; subtitles font styling is cumbersome to adjust.
- **Why Tomato wins:** Intelligent episode folder detection, dedicated "Up Next" queue, automatic sequential naming, and per-series subtitle font presets.

### 3. The Music & Podcast Listener (Elena, 28)
- **Profile:** Plays lossless FLAC files, DJ mixes, audiobooks, and live radio streams while working.
- **Frustrations with VLC:** VLC treats audio files like headless video files, showing an empty cone or clumsy visualizer.
- **Why Tomato wins:** Dedicated audio playback mode with rich album art, artist/album categorization, interactive waveform seek bar, and gapless queue management.

### 4. The Everyday Casual User (David, 45)
- **Profile:** Just wants to double-click an `.mp4`, `.mov`, or `.mkv` file and have it play immediately with zero fuss.
- **Frustrations with VLC:** Overwhelmed by 100+ toolbar buttons, mysterious error popups, and archaic settings.
- **Why Tomato wins:** Instant start, intuitive volume scroll, clean keyboard shortcuts (Space to toggle, arrows to seek), and zero configuration required.

---

## 4. Strategic Non-Goals

To maintain focus, velocity, and architectural integrity, Tomato explicitly rejects the following:
1. **No Cloud-Hosted Streaming Ecosystem:** Tomato is a local desktop player, not a centralized subscription service or SaaS platform.
2. **No Proprietary DRM Enforcement:** Tomato will not implement closed Widevine/PlayReady DRM blobs that restrict user freedom or violate open-source licensing.
3. **No Embedded Web Backend / Bloat:** Tomato will not bundle an invisible Node.js runtime or remote web server when a native desktop binary is orders of magnitude lighter, faster, and more responsive.
4. **No Rewriting Codecs in C++/Rust:** Tomato leverages LibVLC (and FFmpeg underneath). We do not author custom decoders for H.264, HEVC, AV1, or VP9.
5. **No Forced Telemetry or User Accounts:** There is no "Sign in with Tomato" requirement. The application functions completely offline forever.

---

## 5. Architectural Guiding Star

```
┌────────────────────────────────────────────────────────┐
│                   TOMATO DESKTOP APP                   │
├────────────────────────────────────────────────────────┤
│   Presentation Layer: Qt 6 / QML Shell & Custom SVG    │
│   (Near-Black #080909, Tomato Orange #E96914, A11y)    │
├────────────────────────────────────────────────────────┤
│   Application Core: C++20 Domain Logic                 │
│   - Queue Manager      - Subtitle Sync Engine          │
│   - Playlist Store     - Local Media Indexer           │
│   - Settings Service   - Shortcut & Layout Dispatcher  │
├────────────────────────────────────────────────────────┤
│   Media Engine Abstraction (IPlayerEngine)             │
├────────────────────────────────────────────────────────┤
│   LibVLC Adapter (libvlc.dylib / libvlc.so / libvlc)   │
│   (Playback, Hardware Acceleration, Demuxing, Audio)   │
└────────────────────────────────────────────────────────┘
```
