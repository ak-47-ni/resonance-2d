# Resonance 2D Demo Walkthrough

## Goal

Verify the first `resonance-2d` prototype has enough content to demonstrate region-aware atmosphere, event selection, and audio-state changes.

## Steps

1. Build the project from the repository root:
   - `cmake -S . -B build -G Ninja`
   - `cmake --build build`
2. Run the automated content and logic checks:
   - `ctest --test-dir build --output-on-failure`
   - `python3 tools/validate_assets.py`
3. Inspect the current demo data:
   - Regions: `meadow`, `ruins`, `lakeside`, `station`
   - Music states: `explore`, `mysterious`, `calm`
   - Events: `distant_bell`, `old_broadcast_echo`, `shoreline_memory`, `passing_shadow`, `cold_gust`, `echoing_announcement`
4. For the current headless demo slice, confirm the intended traversal logic:
   - Position `(10, 10)` resolves to `meadow`
   - Position `(420, 10)` resolves to `ruins`
   - Position `(999, 999)` resolves to no active region
5. Confirm the intended event and audio logic:
   - Entering `meadow` resolves to `explore`
   - Requesting `mysterious` overrides the region default
   - In `ruins`, the current highest-weight narrative event is `old_broadcast_echo`
   - In `station`, the new region-specific event is `echoing_announcement`

## Current Limitations

- Audio playback is not wired to real mixing yet.
- Debug overlay is currently a text formatter, not an on-screen panel.
- The demo executable is still a lightweight bootstrap, not a full playable scene.

## Next Improvements

- Bind `World`, `EventDirector`, `AudioDirector`, and `DebugOverlay` in one runtime loop.
- Render region and event state in a visible development overlay.
- Start real `SDL3` audio playback through the backend abstraction.
