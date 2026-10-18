# 🍅 Tomato

**Every format. A better experience.**

Tomato is a proposed open-source desktop media player focused on reliable playback, a restrained dark interface, first-class subtitles, and a clean distinction between the active queue and saved playlists.

> **Project status:** early planning and architecture stage. Features are goals until implemented and verified.

## Vision

Tomato aims to combine a capable, proven playback engine with an interface that keeps the media at the center and makes common actions easy to discover.

- Content-first dark UI with Tomato-orange accents.
- Keyboard-friendly playback controls and accessible menus.
- Separate transient playback queue and persistent playlists.
- Local-first media library and preferences.
- Subtitle timing, styling, track selection, and episode matching.
- No required accounts or forced telemetry.

See [docs/VISION.md](docs/VISION.md) for the product vision.

## Proposed architecture

The current working direction is to evaluate a **Qt 6 / QML desktop UI**, a C++ application layer, and an isolated **libVLC adapter**. This is a proposal, not a claim that the application is already implemented. The architecture must be validated with a buildable playback prototype before committing to major implementation.

## Documentation

- [Product vision](docs/VISION.md)
- [Architecture](docs/ARCHITECTURE.md)
- [UI/UX specification](docs/UI_UX_SPEC.md)
- [Roadmap](docs/ROADMAP.md)
- [Architecture decisions](docs/DECISIONS.md)
- [Security and privacy](docs/SECURITY_AND_PRIVACY.md)
- [Build and testing](docs/BUILD_AND_TESTING.md)

## Development status

The repository is currently establishing the product and technical foundations. Check the roadmap for the intended sequence. Do not rely on planned features as available functionality.

## Licensing

The final application license and distribution obligations depend on the chosen implementation, libVLC version, bundled libraries, and assets. Complete a license inventory and review before publishing binary releases. Preserve all upstream copyright and license notices. Tomato is an independent project and is not an official VideoLAN release.

## Contributing

Contributions should be small, testable, and consistent with the architecture and accessibility guidelines. See the documentation before making broad structural changes.
