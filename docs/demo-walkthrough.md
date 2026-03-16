# Resonance 2D Demo Walkthrough

## Goal

Verify the current `resonance-2d` prototype demonstrates region-aware atmosphere, event selection, proximity-based story anchors, and audio-state changes.

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
   - Story anchors: `meadow-swing`, `ruins-gate`, `lakeside-reeds`, `station-bench`
4. In the SDL demo runtime:
   - Move with `WASD` or arrow keys
   - Press `E` near a story anchor when the overlay shows an action prompt
   - Press `J` to toggle the memory journal
   - Press `Esc` to exit
5. Confirm the intended traversal logic:
   - Position `(10, 10)` resolves to `meadow`
   - Position `(420, 10)` resolves to `ruins`
   - Position `(999, 999)` resolves to no active region
6. Confirm the intended event and audio logic:
   - Entering `meadow` resolves to `explore`
   - Requesting `mysterious` overrides the region default
   - In `ruins`, the current highest-weight narrative event is `old_broadcast_echo`
   - In `station`, the region-specific event is `echoing_announcement`
   - Remaining in the same region no longer re-triggers a new major event every frame
   - After a short cooldown, `meadow` rotates away from `distant_bell` toward `passing_shadow`
7. Confirm the new story-anchor interaction logic:
   - At position `(96, 80)`, the meadow prompt appears as `Action: Press E to listen`
   - Triggering that anchor surfaces the meadow-swing story text in the overlay
   - Moving away clears the prompt but keeps the runtime stable
8. Confirm the new story-anchor marker logic:
   - Each anchor is drawn as a small world-space marker
   - Nearby anchors glow warmer than distant anchors
   - Activated anchors gain a bright inner mark

## Current Limitations

- Audio playback is still transitioning from synthesized tones to real assets.
- Debug overlay is intentionally utilitarian and text-first.
- Story anchors are one-shot text reveals, not branching dialogue.

## Next Improvements

- Add visual anchor markers and subtle proximity cues.
- Let story anchors feed future quest or memory systems.
- Blend real audio assets with anchor-triggered ambience changes.

8. Confirm the memory journal logic:
   - Triggering a story anchor records one journal entry
   - Pressing `J` swaps the overlay into `Journal` view
   - Re-triggering the same anchor does not duplicate the journal entry
9. Confirm the memory-driven event unlock logic:
   - Before discovering `meadow-swing`, meadow rotation stays within the default event pool
   - After discovering `meadow-swing`, a later meadow event can resolve to `swing_memory_echo`
10. Confirm the multi-memory event unlock logic:
   - With only `meadow-swing` discovered, station still resolves to `echoing_announcement`
   - After discovering both `meadow-swing` and `ruins-gate`, station can resolve to `platform_convergence`
11. Confirm the three-stage memory chain logic:
   - After also discovering `lakeside-reeds`, station can resolve to `terminal_refrain`
