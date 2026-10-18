# Tomato UI/UX Specification

**Status:** initial design target; validate with an interactive prototype.

## Product principles

1. **The media is the hero.** Keep controls quiet and make them appear when needed.
2. **Discoverability without removing power.** Routine tasks are visible; advanced functions are organized, not deleted.
3. **Predictable interaction.** The same action should work consistently in menus, toolbars, and keyboard shortcuts.
4. **Accessible by default.** Focus states, keyboard interaction, meaningful labels, and contrast are required.
5. **No fake functionality.** A control must perform a real action or be clearly disabled/not yet implemented.

## Color tokens

| Token | Value | Use |
|---|---|---|
| `--tomato-bg` | `#080909` | Main application background |
| `--tomato-canvas` | `#000000` | Video canvas |
| `--tomato-surface` | `#191919` | Menus and elevated surfaces |
| `--tomato-hover` | `#262626` | Hovered/pressed surface |
| `--tomato-accent` | `#E96914` | Primary actions and active state |
| `--tomato-text` | `#F5F3EF` | Primary text |
| `--tomato-muted` | `#A3A3A3` | Secondary text |
| `--tomato-divider` | `#303030` | Borders and separators |

Tokens must be implemented centrally. Check contrast in actual usage, including text and interactive states. Accent color is not the only way to communicate selection or errors.

## Shape and density

- Compact rectangular controls; typically 6–10 px corner rounding.
- Menus/panels may use 10–14 px rounding.
- Consistent spacing scale and alignment.
- Avoid oversized center playback controls that cover the media.
- Hit targets must remain usable on high-DPI and touch-capable systems.
- Motion must be subtle, short, interruptible, and respect reduced-motion preferences when applicable.

## Window regions

### Header
- Tomato branding and a clear media title.
- Native window controls where possible.
- Optional fullscreen and picture-in-picture actions only if supported on the target platform.

### Canvas
- Black background behind video.
- Letterboxing must not distort the media aspect ratio.
- Loading, missing-media, audio-only, and error states need intentional designs.

### Bottom controls
- Left: previous item, play/pause, next item.
- Center: thick seek bar with current and total/remaining time.
- Right: volume, subtitle, audio track/output, queue, settings, fullscreen and supported PiP action.
- Allow a compact layout at narrow window widths without overlapping controls.

### Overlays
- Appear on intentional pointer movement or keyboard interaction.
- Hide after a documented idle delay.
- Do not hide while a menu, dialog, slider drag, or accessible keyboard interaction is active.
- Provide a keyboard shortcut to hide/show controls.

## Menus and panels

Use one consistent hierarchy for common media actions. Avoid duplicating the same long action list across multiple menus. Context menus should be relevant to the media or current view.

Panels should retain playback context when practical and should not cover essential subtitle adjustment controls with oversized modals.

## Icon system

- Use original, consistent SVG assets.
- Standardize SVG viewBox dimensions and stroke weight.
- Two-tone muted gray/orange icons can be used for navigation and menu entries.
- Toolbar controls should use a simpler monochrome form, with clear active/focus/hover states.
- Every interactive icon needs an accessible name and tooltip where useful.
- Do not rely on color alone to communicate status.
- Record licenses and sources for every third-party visual asset.

## Queue and playlists

Label the active queue and saved playlists distinctly in UI, text, and data models. Queue edits must not silently alter a saved playlist. Drag-and-drop targets need visible feedback.

## Subtitles

First release: select embedded tracks and external subtitle files; show selected language; adjust delay and supported styling; expose clear reset/default actions.

Later: folder/playlist matching, per-show presets, provider lookup and optional visual synchronization aids. Remote provider interaction must be documented and user-controlled.

## Accessibility

- Complete keyboard navigation with visible focus.
- Correct accessible names and roles.
- Usable zoom/scaling and high-DPI rendering.
- Sufficient contrast for text, icons, separators, and disabled states.
- Do not communicate errors or selection via color alone.
- Provide non-hover alternatives for actions that are otherwise revealed only by pointer.

## UX acceptance checks

- A first-time user can open a file, pause, seek, select a subtitle/audio track, and find the queue without reading advanced documentation.
- The UI remains usable when resized to a narrow window.
- Menus can be opened and dismissed with keyboard and pointer.
- Fullscreen controls do not permanently obstruct playback.
- Loading/error/empty states are designed and tested.
