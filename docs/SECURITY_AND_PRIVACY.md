# Tomato Security and Privacy

**Status:** initial requirements.

## Baseline

- No required account, cloud backend, analytics, tracking beacon, or telemetry.
- Local media and library features should work offline.
- Ask permission before indexing folders; do not scan an entire home directory or external drive without explicit user action.
- Make history and resume behavior configurable.
- Avoid placing full local paths or sensitive metadata in logs unless needed for an explicit diagnostic, and redact them where practical.
- No remote subtitle, artwork, or metadata lookup without a clear user action or documented opt-in preference.
- Explain what data a provider receives before a remote request.
- Do not execute media files, subtitle files, playlists, or downloaded metadata as code.

## Untrusted inputs

Treat all file and network input as untrusted, including:
- Media filenames and metadata.
- Playlist contents and external stream URLs.
- Subtitle files and subtitle-provider results.
- Cover art, thumbnails, and embedded metadata.
- Network streams and redirects.

Use the media engine's established parsing and security updates. Avoid invented custom parsers where a maintained library exists.

## Filesystem access

- Restrict indexing to user-selected locations.
- Handle disappearing drives, symlinks, unreadable folders, and malformed files.
- Avoid destructive file actions.
- Confirm before overwriting or deleting user data.
- Keep database and cache paths predictable for each operating system.

## Network

- Validate URLs and supported schemes.
- Apply sensible timeouts and cancellation to network operations.
- Clearly distinguish local files from network streams.
- Do not silently download subtitles or metadata.
- Document provider endpoints and third-party policies for any optional remote integrations.

## Dependencies and releases

- Track third-party dependencies and licenses.
- Keep bundled runtime components updated.
- Do not claim security hardening without relevant tests/review.
- Publish a security contact or reporting process before broad release.
