# Tomato Architecture Decisions

Keep decisions concise and update them when evidence changes.

## ADR-001 — Treat the UI and media engine as separate layers

**Status:** proposed.

**Decision:** expose playback through an application-level interface rather than calling engine APIs directly throughout UI components.

**Reason:** makes lifecycle, event handling, testing, and future engine changes more manageable.

**Trade-off:** an adapter introduces code and must accurately represent engine capabilities.

**Validation:** implement open/play/pause/seek/track selection and exercise event and shutdown behavior with real media.

## ADR-002 — Evaluate a standalone Qt/QML application first

**Status:** under evaluation.

**Context:** Tomato has a substantially different UI and product information architecture from the existing VLC desktop interface.

**Candidate:** a standalone Qt 6/QML shell with C++ core and a libVLC adapter.

**Alternative:** modify VLC's existing Qt UI inside a fork.

**Validation required:** compare a small prototype, build requirements, supported platforms, library APIs, release maintenance, and exact licensing/redistribution implications. Do not treat this hypothesis as final until reviewed.

## ADR-003 — Local-first, no required account or telemetry

**Status:** proposed.

**Decision:** the primary playback and library experience should work locally without an account; no forced analytics or telemetry.

**Reason:** local media paths, viewing history, and metadata can be sensitive.

**Trade-off:** remote metadata and subtitle providers need explicit, visible user choice and clear error handling.

## ADR-004 — Queue and playlists are different domain concepts

**Status:** proposed.

**Decision:** current-session queue order is separate from saved, named playlists.

**Reason:** avoids accidental changes to user collections when changing what plays next.

**Validation:** unit and persistence tests for reorder, duplicate entries, deletion, restart, and playlist export.
