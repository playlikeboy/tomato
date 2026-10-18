# 🤝 Contributing to Tomato

Thank you for your interest in contributing to **Tomato**! We are building a modern, beautifully designed, open-source desktop media player that combines VLC's universal playback capabilities with a clean, discoverable, and accessible interface.

---

## 1. Code of Conduct

Tomato is dedicated to providing a welcoming, inclusive, and harassment-free experience for everyone. Please be respectful, constructive, and collaborative in all communications.

---

## 2. Architectural Principles to Keep in Mind

Before contributing code, please review [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) and [docs/DECISIONS.md](docs/DECISIONS.md):
1. **Engine Decoupling:** Presentation and domain logic components must never include `vlc/vlc.h` directly. All playback operations must flow through `engine/player-interface/player_interface.h` (`IPlayerEngine`).
2. **Strict Queue vs. Playlist Distinction:** The ephemeral session queue (`QueueManager`) and persistent playlists (`PlaylistStore`) are distinct models with different lifecycles.
3. **Local-First & Privacy First:** No tracking, telemetry, pingbacks, or unexpected filesystem indexing.

---

## 3. Getting Started

### Prerequisites
- Modern C++20 compiler (Apple Clang 15+, GCC 12+, MSVC 2022+)
- CMake 3.22+
- LibVLC 3.0.x SDK (e.g. from `/Applications/VLC.app` on macOS or `libvlc-dev` on Linux)

### Environment Check
Run the automated environment check script:
```bash
./scripts/check_env.sh
```

### Building & Running Tests
```bash
cmake -B build -S . -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j4
ctest --test-dir build --output-on-failure
```

---

## 4. Development Workflow

### Branch Strategy
- `main`: The stable production branch.
- Feature branches: `feature/short-description` (e.g., `feature/tapered-volume-cone`).
- Bugfix branches: `fix/issue-description` (e.g., `fix/subtitle-offset-step`).

### Commit Message Conventions
Tomato enforces **Conventional Commits**:
- `feat(player): add tapered volume cone widget`
- `fix(queue): prevent index out of bounds on rapid clear`
- `docs(spec): update keyboard shortcut table in UI_UX_SPEC`
- `refactor(engine): isolate libvlc event callbacks in worker thread`
- `test(queue): add unit tests for shuffle and repeat transitions`

### Developer Certificate of Origin (DCO)
All commits must be signed off to certify developer origin:
```bash
git commit -s -m "feat(player): add tapered volume slider"
```

---

## 5. Code Style & Asset Guidelines

- **C++ Formatting:** Use the repository `.clang-format` (Google / LLVM style with 4-space indentation).
- **SVG Icon Assets:**
  - ViewBox: `0 0 24 24`
  - Stroke: `1.75px` or `2.0px`, `stroke-linecap="round"`, `stroke-linejoin="round"`
  - Colors: Use CSS variables (`color-text-secondary`, `color-accent-primary`) or `currentColor`.

---

## 6. Submitting a Pull Request

1. Fork the repository and create your feature branch from `main`.
2. Ensure all existing tests pass (`ctest --test-dir build`).
3. Add unit tests for new logic (e.g., in `tests/`).
4. Submit your pull request with a clear description, linking any relevant GitHub issues.
