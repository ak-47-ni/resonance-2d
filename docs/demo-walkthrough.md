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
   - Event mix profiles: meadow events now carry authored `event_duck` and `ambient_boost` values for future audio-direction use
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
8. Confirm the new story-focus audio logic:
   - Standing on `meadow-swing` produces the strongest story-focus response
   - Moving about halfway toward the edge of that anchor radius softens the effect
   - Leaving the anchor radius returns the audio mix to its neutral state
9. Confirm the new event-emphasis audio logic:
   - When a fresh narrative event is selected, the mix briefly ducks the BGM and lifts the ambience
   - The emphasis decays over the next few updates instead of snapping off immediately
   - Story-focus and event emphasis can stack without breaking event rotation
10. Confirm the new event mix profile logic:
   - `distant_bell` resolves with a stronger authored mix profile than `passing_shadow`
   - Meadow event rotation changes not just the event id but also the active authored mix profile
   - Authored `event_duck` / `ambient_boost` values now flow into the resolved runtime audio gains
   - Debug summary / hidden demo stdout now surfaces `StoryFocus`, `EventEmphasis`, `EventDuck`, `AmbientBoost`, `BgmGain`, `AmbientGain`, `Memories`, `LatestMemory`, `ChainStage`, `StationChain`, `NextMemory`, `NextStationUpgrade`, `VisibleEvents`, and `NextEventUnlock` for quick terminal-side verification
11. Confirm the new story-anchor marker logic:
   - Each anchor is drawn as a small world-space marker
   - Nearby anchors glow warmer than distant anchors
   - Activated anchors gain a bright inner mark
   - The overlay now shows `Nearby Anchor:` before interaction and `Active Anchor:` after a memory is triggered

## Current Limitations

- Audio playback now uses looped assets plus lightweight story-focus shaping and event-emphasis fades, but still lacks richer adaptive composition.
- Debug overlay is intentionally utilitarian and text-first.
- Story anchors are one-shot text reveals, not branching dialogue.

## Next Improvements

- Extend event emphasis into authored per-event mix profiles.
- Let story anchors feed future quest or memory systems.
- Add richer on-screen debug text and anchor feedback.

12. Confirm the memory journal logic:
   - Triggering a story anchor records one journal entry
   - Pressing `J` swaps the overlay into `Journal` view
   - Re-triggering the same anchor does not duplicate the journal entry
   - Each journal entry now shows its source `Region:` line for quick narrative context
13. Confirm the memory-driven event unlock logic:
   - Before discovering `meadow-swing`, meadow rotation stays within the default event pool
   - After discovering `meadow-swing`, a later meadow event can resolve to `swing_memory_echo`
14. Confirm the multi-memory event unlock logic:
   - With only `meadow-swing` discovered, station still resolves to `echoing_announcement`
   - After discovering both `meadow-swing` and `ruins-gate`, station can resolve to `platform_convergence`
15. Confirm the three-stage memory chain logic:
   - After also discovering `lakeside-reeds`, station can resolve to `terminal_refrain`
   - Debug overlay/summary expose `ChainStage=0/3..3/3` and the current `StationChain` hint while you progress the memory chain
   - Unlock-hint telemetry now also shows the next required memory and the next station upgrade target while progressing the chain
   - Region event-pool telemetry now shows the currently visible event IDs and the next locked event plus its missing memory tags
   - Journal view now mirrors the same chain progression and event unlock hints, so `J` still exposes the next narrative milestone
16. Confirm the hidden memory-chain route helper:
   - Run `SDL_AUDIODRIVER=dummy SDL_VIDEODRIVER=dummy RESONANCE_DEMO_HIDDEN=1 RESONANCE_DEMO_ROUTE=memory_chain build/resonance_demo`
   - Output should include `RouteStep=meadow-start`, `RouteStep=station-stage-two`, and `RouteStep=station-stage-three`
   - The final route line should show `Event=terminal_refrain` and `NextEventUnlock=<none>`
