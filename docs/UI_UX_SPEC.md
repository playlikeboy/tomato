# 🎨 Tomato — UI/UX Specification & Design System

> **Document Version:** 1.0.0  
> **Target Release:** v0.1.0-alpha  
> **Status:** Approved Design Baseline  

---

## 1. Design Philosophy & Visual Language

Tomato's visual language is **understated, cinematic, and deliberate**. The interface never competes with the content on screen. When video is playing, controls remain quiet and compact; when invoked, every interaction is responsive, precise, and visually grounded in a consistent dark aesthetic.

---

## 2. Design Tokens

### Color Palette

| Token Name | Hex Value | Usage / Description |
| :--- | :--- | :--- |
| `color-bg-primary` | `#080909` | Deepest background surface; window shell background |
| `color-bg-canvas` | `#000000` | True-black media canvas; letterboxing area |
| `color-surface-elevated` | `#191919` | Elevated surfaces: playback bar, context menus, drawers |
| `color-surface-hover` | `#262626` | Interactive button hover state, menu item hover |
| `color-surface-active` | `#202020` | Pressed button state |
| `color-accent-primary` | `#E96914` | Tomato Orange: play button, timeline progress, active states |
| `color-accent-hover` | `#F27A2B` | Hover on primary accent buttons |
| `color-accent-active` | `#D05A0C` | Pressed primary accent buttons |
| `color-text-primary` | `#F5F3EF` | Primary headers, active titles, high-contrast labels |
| `color-text-secondary` | `#A3A3A3` | Toolbar icons, timestamps, secondary labels, shortcuts |
| `color-text-muted` | `#666666` | Disabled states, empty placeholder hints |
| `color-border-divider` | `#303030` | Menu dividers, panel separation lines |
| `color-border-subtle` | `#222222` | Outer card borders, window container stroke |
| `color-indicator-record`| `#E53935` | Recording active indicator, critical error alerts |
| `color-focus-ring` | `rgba(233, 105, 20, 0.45)` | Accessible focus outlines (3px spread) |

### Corner Geometry & Radii

| Element Type | Radius Value | Notes |
| :--- | :--- | :--- |
| **Standard Controls** | `8px` (range: 6–10px) | Buttons, input fields, badges, timeline scrubber cap |
| **Menus & Floating Panels** | `12px` (range: 10–14px) | Context menu, settings dropdowns, tooltip containers |
| **Drawers & Modals** | `14px` | Queue drawer, Subtitle workspace, preferences dialog |
| **Play/Pause Hero Button** | `8px` | Distinct Tomato-orange rounded rectangular container |

### Typography

- **Primary Font Family:** System UI default (`-apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, "Inter", sans-serif`) to ensure native rendering fidelity across macOS, Windows, and Linux.
- **Monospace Font Family:** `"SF Mono", "Consolas", "Liberation Mono", monospace` for timestamps (`00:00:02 / 00:25:40`), bitrate counters, and subtitle delay readouts (`+150 ms`).

| Text Style | Size | Weight | Line Height | Usage |
| :--- | :--- | :--- | :--- | :--- |
| **Title / Heading** | `15px` | 600 (SemiBold) | 20px | Header title, drawer headers |
| **Body / Labels** | `13px` | 400 (Regular) | 18px | Menu items, button tooltips, settings text |
| **Body Emphasized** | `13px` | 500 (Medium) | 18px | Active track names, selected options |
| **Timestamps / Code** | `12px` | 500 (Monospace) | 16px | Current time, total duration, delay offset |
| **Caption / Hotkey** | `11px` | 400 (Regular) | 14px | Context menu shortcut hints (`Space`, `Ctrl+Q`) |

---

## 3. Custom SVG Icon System

To eliminate mismatched visual assets, Tomato employs an original, coherent vector icon system.

### Geometry Specifications
- **ViewBox:** Uniform `0 0 24 24` for all icons.
- **Stroke Width:** `1.75px` default (`2.0px` for small indicators).
- **Strokes:** Rounded line caps (`stroke-linecap="round"`) and rounded line joins (`stroke-linejoin="round"`).
- **Filling:** Default `none`, with deliberate accent fills for key states.

### Dual Treatment Pattern
1. **Toolbar Icons (Monochrome):**
   - Clean, lightweight outline icons rendered in `color-text-secondary` (`#A3A3A3`).
   - Dynamically transition to `color-accent-primary` (`#E96914`) on hover, keyboard focus, and when toggled active.
2. **Context Menu & Navigation Icons (Two-Tone):**
   - Structural lines rendered in muted gray (`#A3A3A3`).
   - Distinct secondary visual cues (such as playback arrows, audio wave crests, or subtitle text lines) accented in Tomato Orange (`#E96914`).

---

## 4. Primary Playback Window Layout

```
┌──────────────────────────────────────────────────────────────────────────────┐
│ 🍅  The Wand S01E04                                              —  □  ✕     │ Top Bar
├──────────────────────────────────────────────────────────────────────────────┤
│                                                                              │
│                                                                              │
│                             VIDEO CANVAS (#000000)                           │
│                                                                              │
│                                                                              │
│                                ┌───────────────────────────┐                 │
│                                │ ▷ Play              Space │                 │
│                                │ □ Stop                  S │ Context Menu    │
│                                │ ◁ Previous              P │ (Right Click)   │
│                                │ ▷ Next                  N │                 │
│                                │ ◉ Record                R │                 │
│                                │ ───────────────────────── │                 │
│                                │ ▷ Playback              > │                 │
│                                │ ▤ Subtitle              > │                 │
│                                │ ⚙ More Settings         > │                 │
│                                └───────────────────────────┘                 │
│                                                                              │
├──────────────────────────────────────────────────────────────────────────────┤
│ ──[===========================○───────────────────────────────────────────]─ │ Seek Bar
│                                                                              │
│  [◁] [ ▷ ] [▷]  00:00:02                               00:25:40  ◁ [ ▂▃▅█] ▤ ♬ ☰ ⛶ ⚙ │ Control Row
└──────────────────────────────────────────────────────────────────────────────┘
```

### 1. Top Bar
- **Height:** 38px.
- **Components:** Tomato brand mark (left), active media title with auto-truncation (center), window management controls (minimize, maximize, close) on the right (matching macOS Cocoa traffic lights on Apple Silicon, or Win32 controls on Windows).
- **Behavior:** Fades to 0% opacity during fullscreen playback when the pointer is stationary.

### 2. Video Canvas
- True `#000000` surface.
- Smooth letterboxing and pillarboxing.
- Double-click toggles fullscreen.
- Single-click pauses/resumes playback (with subtle center ripple feedback).
- Scroll wheel modulates volume up/down.

### 3. Seek Bar (Timeline)
- **Thickness:** `6px` at rest; expands smoothly to `10px` on pointer hover or active drag.
- **Elapsed Track:** Rendered in `color-accent-primary` (`#E96914`).
- **Remaining Track:** Rendered in elevated divider tone (`#303030`).
- **Scrubber Thumb:** `14px` circular thumb with a `2px` white core and orange glow, appearing on hover.
- **Hover Preview Tooltip:** Floating badge displaying exact timestamp `hh:mm:ss` (and future thumbnail preview).

### 4. Bottom Control Row
- **Height:** 48px, positioned cleanly below or floating over bottom edge with elevated surface styling.
- **Left Action Group:**
  - `Previous Item` (32x32px hit area)
  - `Play / Pause Hero Button` (36x32px rectangular tomato-orange button with play/pause glyph)
  - `Next Item` (32x32px hit area)
  - `Elapsed Time` (`00:00:02`, monospace typography)
- **Right Action Group:**
  - `Remaining / Total Duration` (`00:25:40`)
  - `Tapered Volume Slider` (Cone-inspired graduated volume wedges)
  - `Subtitle Workspace Toggle` (Opens subtitle drawer)
  - `Audio Track Switcher` (Pop-over audio menu)
  - `Active Queue Drawer Toggle`
  - `Fullscreen Toggle`
  - `Quick Preferences / Cog`

---

## 5. Signature UI Components

### 1. The Cone-Inspired Tapered Volume Slider
In homage to VLC's iconic heritage, Tomato features a **tapered horizontal volume control**:
- A stylized audio cone / wedge geometry that widens from left (0% volume) to right (100% / 125% boost).
- Fills smoothly with Tomato Orange up to 100%, transitioning to a warning amber for software amplification over 100%.
- Mouse wheel anywhere over the video surface or volume slider adjusts volume in 5% increments.

### 2. Subtitle Workspace
Clicking the Subtitle icon slides open the dedicated Subtitle Workspace:
- **Delay Calibration Slider:** Large, centered slider with millisecond precision (`-5000 ms` to `+5000 ms`), including `[-] 50ms` and `[+] 50ms` fine-tuning step buttons.
- **Active Track Selector:** Clear list of embedded and external tracks with language labels (e.g., `English [SDH]`, `Japanese (Full)`).
- **Visual Styler Preview:** Live font selection, font size scale (50% to 200%), text color, background outline, and vertical positioning offset.
- **Episode Matcher:** Instant status showing whether subtitle files were detected in the same directory.

### 3. Context Menu
Right-clicking anywhere on the playback canvas opens the unified context menu:
- Frosted dark backdrop (`#191919` at 95% opacity with 16px background blur).
- Smooth entry animation (fade & 4px vertical scale).
- Top group: Immediate playback controls with visible keyboard shortcuts (`Play Space`, `Stop S`, `Previous P`, `Next N`, `Record R`).
- Middle group: Category submenus (`Playback`, `Video`, `Audio`, `Subtitle`, `More Settings`, `Playlist`, `Open Media`).
- Footer: `Quit Tomato` (`Ctrl+Q` / `Cmd+Q`).

---

## 6. Keyboard Shortcuts Map

| Action | Primary Shortcut | Secondary Shortcut |
| :--- | :--- | :--- |
| **Play / Pause** | `Space` | `K` |
| **Stop Playback** | `S` | `Esc` (in windowed mode) |
| **Seek Forward 5s** | `Right Arrow` | `L` |
| **Seek Backward 5s** | `Left Arrow` | `J` |
| **Seek Forward 30s** | `Ctrl + Right Arrow` | `Shift + Right Arrow` |
| **Seek Backward 30s** | `Ctrl + Left Arrow` | `Shift + Left Arrow` |
| **Volume Up (+5%)** | `Up Arrow` | `Mouse Wheel Up` |
| **Volume Down (-5%)**| `Down Arrow` | `Mouse Wheel Down` |
| **Mute / Unmute** | `M` | — |
| **Fullscreen Toggle**| `F` | `Double Click Canvas` |
| **Hide UI Overlays** | `H` | — |
| **Next Queue Item** | `N` | `Ctrl + Right Bracket` |
| **Prev Queue Item** | `P` | `Ctrl + Left Bracket` |
| **Subtitle Delay -50ms** | `G` | `[` |
| **Subtitle Delay +50ms** | `H` | `]` |
| **Audio Delay -50ms** | `J` | — |
| **Audio Delay +50ms** | `K` | — |
| **Cycle Audio Tracks** | `B` | — |
| **Cycle Subtitle Tracks**| `V` | — |
| **Open File Dialog** | `Ctrl + O` / `Cmd + O` | — |
| **Open Queue Drawer** | `Q` | — |
| **Open Settings** | `Ctrl + ,` / `Cmd + ,` | — |
| **Quit Tomato** | `Ctrl + Q` / `Cmd + Q` | — |

---

## 7. Accessibility (A11y) & Usability

Tomato adheres to **WCAG 2.1 Level AA** standards:
1. **Contrast Ratios:**
   - Primary text (`#F5F3EF`) on primary surface (`#080909`): **18.4:1** (Far exceeds 7:1 AAA requirement).
   - Primary accent (`#E96914`) on elevated surface (`#191919`): **5.2:1** (Exceeds 4.5:1 AA requirement).
2. **Keyboard Focus Navigation:**
   - Full keyboard accessibility across all interactive controls.
   - Distinct, glowing 3px focus ring (`rgba(233, 105, 20, 0.45)`) indicates active focus state.
3. **Screen Reader Labels:**
   - All custom SVG controls possess explicit accessible names (e.g., `aria-label="Play media"`, `aria-valuenow="42"` for timeline progress).
