# Tomato Architecture

**Status:** proposed architecture; validate before major implementation.

## Goals

- Keep the UI decoupled from media-engine lifecycle management.
- Use a real, established media engine rather than implementing codecs.
- Keep library indexing, playback queue, saved playlists, preferences, and metadata logic testable outside presentation components.
- Preserve a local-first and privacy-respecting application.
- Make failures, loading states, and resource cleanup explicit.

## Architecture decision to validate

Evaluate two paths before committing:

1. **Standalone Tomato app:** Qt 6/QML UI and C++ application core around an isolated libVLC adapter.
2. **Tomato UI inside a VLC fork:** modify the existing Qt/QML UI in VLC's source tree.

Compare build feasibility, API coverage, platform support, UI flexibility, packaging, release maintenance, licensing, and ability to keep downstream changes manageable. The current hypothesis is the standalone app, subject to a working playback prototype.

## Proposed layers

### Presentation — Qt Quick / QML

Responsible for window composition, reusable controls, menus, drawers, accessibility labels, responsive layout, visual state, and user input.

Presentation components must not directly manage libVLC handles or implement persistence logic.

### Application core — C++ domain services

Proposed services:

- `PlaybackService`: coordinates player commands and lifecycle.
- `QueueService`: session queue and ordering.
- `PlaylistService`: saved named collections and import/export.
- `LibraryService`: user-authorized folder scanning and media browsing.
- `SubtitleService`: available tracks, external subtitle selection, timing and style preferences.
- `PreferencesService`: validated settings and migrations.
- `HistoryService`: configurable recent-media and resume state.

Keep services narrowly scoped. Do not build all of these at once; introduce them when their vertical slice is scheduled.

### Engine integration — libVLC adapter

Define an interface that hides engine-specific types from most of the app. Likely operations include:

- Open media and release it safely.
- Play, pause, stop, and seek.
- Read current time, duration, state, and errors.
- Read/select audio and subtitle tracks where supported.
- Set volume and playback rate where supported.
- Subscribe to events without touching UI objects from unsafe threads.

Verify exact capabilities against the selected libVLC API and target platforms. Engine callbacks must be marshalled safely to the UI thread. Ensure all callbacks and native resources are disconnected/released during close and shutdown.

### Persistence

Select storage only after enumerating the first-release data model. SQLite is a candidate for library metadata and preferences; portable playlist formats such as M3U8 may be useful for interoperability. Avoid persisting transient queue state by default unless the product explicitly opts into it.

Use migrations, error handling, atomic writes where appropriate, and clear ownership of user data.

### Assets and design system

Keep original Tomato logos, SVG icons, design tokens, and themes in a well-documented assets area. Treat asset licensing as part of the license inventory.

## Suggested source layout

This is a starting proposal, not a required structure:

```text
app/
  shell/
  player/
  library/
  queue/
  playlists/
  subtitles/
  audio/
  settings/
core/
  playback/
  media/
  preferences/
  persistence/
engine/
  player-interface/
  libvlc-adapter/
assets/
  icons/
  logo/
  themes/
tests/
docs/
```

Match final directory naming and build integration to the actual project generator and Qt conventions.

## Threading and lifecycle

- Never block UI rendering for folder scans, metadata reads, or network requests.
- All background work must support cancellation where practical.
- Avoid calling UI objects directly from engine callbacks or worker threads.
- Handle rapid open/close, playback completion, seeking during loading, and app shutdown.
- Surface recoverable errors in the UI; keep diagnostics useful without leaking sensitive filesystem paths unnecessarily.

## Non-functional requirements

- Works offline for local playback.
- No mandatory account, cloud service, analytics, or telemetry.
- Keyboard navigation and visible focus.
- High-DPI and resizable windows.
- Automated tests for core state transitions.
- Platform-specific builds verified before being advertised as supported.

## Open questions

1. Which target platforms are required for the first release?
2. Which Qt and libVLC versions can be built and packaged reliably?
3. Which formats and track features are essential for the first vertical slice?
4. What are the actual redistribution and license requirements for each platform?
5. Is the first milestone a prototype, an MVP, or a forked VLC UI experiment?
