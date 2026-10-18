# Tomato Roadmap

**Status:** proposed order. Dates and estimates are intentionally omitted until the architecture and build environment are validated.

## Phase 0 — Repository audit and feasibility

- [ ] Confirm source repository, branch, working tree, and baseline commit.
- [ ] Compare standalone Qt/QML + libVLC app with modifying VLC's Qt UI.
- [ ] Verify Qt, C++ toolchain, libVLC headers/runtime, and packaging availability on target platforms.
- [ ] Create architecture, UX, privacy, build, and licensing documents.
- [ ] Identify one small, redistributable media test file and expected behavior.
- [ ] Decide first-release platforms and supported scope.

**Exit criteria:** documented architecture decision, a verified build strategy, and a clear first vertical slice.

## Phase 1 — Brand and shell

- [ ] Establish original Tomato logo and app icon.
- [ ] Implement design tokens and consistent SVG icon guidelines.
- [ ] Build the main window, header, canvas placeholder, compact playback bar, menus, and settings shell.
- [ ] Add keyboard focus states and basic accessibility.
- [ ] Make unsupported controls visibly unavailable rather than pretending to work.

**Exit criteria:** runnable interface shell with responsive window layout and no dead interactive controls.

## Phase 2 — Real playback vertical slice

- [ ] Implement media-engine adapter and lifecycle.
- [ ] Open a local media file.
- [ ] Play, pause, stop, seek, and control volume.
- [ ] Display time, duration, loading and error states.
- [ ] Select supported audio and subtitle tracks.
- [ ] Handle end of playback, rapid file changes, and application shutdown.

**Exit criteria:** integration tests verify the basic playback path with a known test fixture.

## Phase 3 — Queue, playlists and library basics

- [ ] Add a distinct session queue.
- [ ] Reorder, skip, remove and clear queue items.
- [ ] Save and reopen named playlists.
- [ ] Add explicit user-selected library folders.
- [ ] Search, sort and change library view.
- [ ] Add configurable recent-media/resume behavior.

**Exit criteria:** state and persistence tests demonstrate predictable queue and playlist behavior.

## Phase 4 — Subtitle/audio workflow

- [ ] Open an external subtitle file.
- [ ] Adjust subtitle delay and supported styling.
- [ ] Make audio-track selection clear.
- [ ] Add default track/language preferences.
- [ ] Research folder/playlist subtitle matching.
- [ ] Evaluate provider integrations, privacy, and terms before implementation.
- [ ] Prototype any subtitle timeline/visual synchronization feature before committing to it.

**Exit criteria:** core local subtitle operations are tested; any remote features are explicitly documented and opt-in.

## Phase 5 — Preferences, customization and polish

- [ ] Searchable preferences and progressive disclosure.
- [ ] Keyboard shortcut reference and conflict handling.
- [ ] Theme/accent selection.
- [ ] Toolbar customization and named profiles, if supported by the chosen architecture.
- [ ] Accessibility, localization, high-DPI and narrow-window review.
- [ ] Profile and settings migration tests.

**Exit criteria:** usability review plus automated coverage for core settings behavior.

## Phase 6 — Network workflows and release hardening

- [ ] Add network stream opening with clear validation/errors.
- [ ] Audit resource limits, timeouts and unsafe file/URL handling.
- [ ] Build platform packages and file associations as appropriate.
- [ ] Add CI for formatting, tests, and supported builds.
- [ ] Complete third-party license inventory and attribution.
- [ ] Document install, upgrade, rollback, debugging and support workflows.
- [ ] Test crash recovery and resource cleanup.

**Exit criteria:** release candidates are built and tested on each platform claimed as supported.

## Deferred until justified

- Cloud accounts, cloud synchronization, remote backend, and telemetry.
- In-house codec implementation.
- Automatic subtitle download without explicit user preference.
- Broad plugin ecosystems and custom scripting.
- Platforms that lack a verified build and test path.
