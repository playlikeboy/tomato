# 🛡️ Tomato — Security & Privacy Architecture

> **Document Version:** 1.0.0  
> **Status:** Active Security Baseline  

---

## 1. Core Security & Privacy Philosophy

**Tomato** is designed from the ground up as a **local-first, privacy-respecting media player**. Unlike modern cloud media platforms, Tomato treats your personal media collection as private and confidential.

### Guiding Tenets
1. **Zero Telemetry:** No usage metrics, playback habits, crash telemetry, or user identifiers are ever collected or transmitted.
2. **Local Data Sovereignty:** Watch history, resume points, playlists, and settings remain solely on your local filesystem.
3. **Explicit Consent for Network Activity:** The player never accesses the internet unless the user explicitly opens a remote URL or initiates an external subtitle search.

---

## 2. Filesystem Security & Sandboxing

### Media Library Scanning
- **Explicit Folder Boundaries:** Tomato never scans or indexes the user's entire filesystem (`/` or `C:\`). It only indexes directories explicitly designated by the user in the Media Library settings.
- **Path Traversal Prevention:** All file path resolutions sanitize `../` sequences to prevent traversal attacks when processing external playlist files (`.m3u` / `.pls`).
- **Symlink Protection:** The local scanner detects and ignores circular directory symlinks to prevent infinite loops or memory exhaustion.

### Data Storage Locations
Tomato strictly adheres to operating system conventions for application data and cache storage:

| Platform | Configuration / Settings | Cache (Thumbnails & Indexes) |
| :--- | :--- | :--- |
| **macOS** | `~/Library/Application Support/Tomato/` | `~/Library/Caches/Tomato/` |
| **Linux** | `~/.config/tomato/` (`$XDG_CONFIG_HOME`) | `~/.cache/tomato/` (`$XDG_CACHE_HOME`) |
| **Windows** | `%APPDATA%\Tomato\` | `%LOCALAPPDATA%\Tomato\Cache\` |

File permissions on local databases and configuration files are restricted to the current operating system user (`0600` for files, `0700` for directories).

---

## 3. Network Communication & Privacy Guardrails

```
┌────────────────────────────────────────────────────────┐
│                   TOMATO DESKTOP APP                   │
├────────────────────────────────────────────────────────┤
│  [Local Media Playback]        ──► ZERO Network Calls  │
│  [Active Queue & Playlists]    ──► ZERO Network Calls  │
│  [Subtitle Delay & Styling]    ──► ZERO Network Calls  │
│  [Preferences & Settings]      ──► ZERO Network Calls  │
├────────────────────────────────────────────────────────┤
│  [Network Stream URL]          ──► Direct to User URL  │
│  [Remote Subtitle Search]      ──► Explicit User Click │
└────────────────────────────────────────────────────────┘
```

### Network Stream Playback
- Supported protocols: `http://`, `https://`, `rtsp://`, `rtmp://`, `mms://`.
- User streams are negotiated directly through LibVLC's native network modules. Tomato introduces no intermediate proxy servers.
- **Privacy-Preserving User-Agent:** The default HTTP User-Agent string is minimal (`Tomato/<version> (LibVLC/<version>)`), omitting machine hardware IDs, OS patch levels, or MAC addresses.

### Remote Subtitle Provider Access (Phase 4.4+)
- **Explicit User Action:** Remote subtitle searches are triggered only by explicit button click; Tomato will never scan or query external APIs automatically on media open.
- **Metadata Leakage Prevention:** Queries send only the minimal media hash or clean sanitized title. Local folder paths, usernames, and file timestamps are stripped.
- **HTTPS Enforcement:** All remote provider communication occurs exclusively over encrypted TLS connections.

---

## 4. Subtitle File Parsing Safety

Subtitle formats such as SubStation Alpha (`.ass` / `.ssa`) allow extensive styling, positioning, and embedded scripting fonts.
- Tomato delegates complex subtitle parsing to the security-hardened `libass` and LibVLC rendering pipelines, which are actively monitored for memory safety vulnerabilities.
- Subtitle parsing runs with bounded memory limits to mitigate malicious file denial-of-service attempts.

---

## 5. Local Logging & Crash Diagnostics

- **Diagnostic Logs:** Tomato maintains a rotating local log file (`tomato.log`) in the user data directory, retaining a maximum of 5 MB across 2 backup rotations.
- **Zero Auto-Upload:** Diagnostic logs and crash stack traces are stored purely locally. No automated crash reporter daemon runs in the background.
- If a user wishes to submit a bug report on GitHub, they can export a sanitized diagnostic report with file paths redacted.

---

## 6. Open Source Licensing & Third-Party Integrity

- **LibVLC:** Licensed under **LGPL-2.1-or-later**. Dynamic linking against `libvlc.dylib` / `libvlc.so` / `libvlc.dll` complies with LGPL terms, allowing end users to swap or re-link the underlying engine.
- **Tomato Application:** Licensed under the permissive **MIT License** or **Apache-2.0**, promoting open community contribution and educational transparency.
- **Attribution:** Upstream VideoLAN copyrights and author notices are retained and honored in the `LICENSE` and `docs/` directories.
