# Build and Testing Guide

**Status:** discovery checklist; no build command is considered verified until run in the target environment.

## Before building

Record:
- Operating system and version.
- Architecture (for example arm64 or x86_64).
- Compiler and C++ standard support.
- Qt version and required modules.
- libVLC development headers and runtime version.
- Build generator and package manager.
- Target platforms and packaging requirements.

Do not assume VLC's source repository and a standalone Tomato repository share the same build instructions.

## Standalone application candidate

1. Install a supported Qt 6 SDK and C++ toolchain for the target system.
2. Determine whether libVLC development files are available natively or must be built/packaged.
3. Create the smallest Qt Quick application that renders a window.
4. Add a small media view and integrate exactly one local test file through the proposed adapter.
5. Verify play, pause, seek, basic events, errors, and clean resource release.
6. Document tested versions and exact commands only after they succeed.

## VLC fork candidate

Use the current VLC installation/build documentation and Qt interface hacking guide. Preserve the existing build-system choices during the initial experiment. Check dependencies and platform-specific instructions before attempting a full build.

## Test tiers

### Unit
Queue operations, playlist behavior, preferences, migrations, file matching and pure domain logic.

### Integration
Known media fixture; open, playback, seeking, end-of-file, track selection, failed media and shutdown.

### UI
Keyboard navigation, accessible labels, menu operation, window resize, fullscreen overlay, dialogs, loading/error/empty states.

### Release
Repeatable builds, packaged application launch, file associations, dependency/license inventory, and platform smoke tests.

## Reporting

For every validation run, record the exact command, environment, exit status, and important output. Distinguish:
- not run;
- blocked by environment;
- failed;
- passed.

Never report a build or test as passed based only on code inspection.
