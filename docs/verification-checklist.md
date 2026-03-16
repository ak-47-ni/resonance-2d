# Resonance 2D MVP Verification Checklist

## Automated Verification

Run from the repository root:

- `cmake -S . -B build -G Ninja`
- `cmake --build build`
- `ctest --test-dir build --output-on-failure`
- `/opt/homebrew/bin/python3 tools/validate_assets.py`

Expected results:

- Build completes successfully.
- All tests pass.
- Asset validation prints `Asset validation passed`.
- Hidden demo stdout includes `StoryFocus=`, `EventEmphasis=`, `EventDuck=`, `AmbientBoost=`, `BgmGain=`, `AmbientGain=`, `Memories=`, `LatestMemory=`, `ChainStage=`, `StationChain=`, `NextMemory=`, `NextStationUpgrade=`, `VisibleEvents=`, `NextEventUnlock=`, and `NearbyAnchor=` once `resonance_demo` is rebuilt.
- Journal overlay includes `Memory Chain:`, `Station Chain:`, `Next Memory:`, `Next Station Upgrade:`, `Visible Events:`, and `Next Event Unlock:` after toggling `J`.
- Runtime overlay includes `Nearby Anchor:` when the player is in range and `Active Anchor:` after a story anchor has been triggered.
- Journal entries now include `Region:` lines in journal view, matching the stored `MemoryJournalEntry.region_id`.

## Manual Logic Verification

Run hidden demo stdout verification from the repository root:

- `cmake --build build --target resonance_demo`
- `SDL_AUDIODRIVER=dummy SDL_VIDEODRIVER=dummy RESONANCE_DEMO_HIDDEN=1 RESONANCE_DEMO_AUTOCLOSE=1 build/resonance_demo`

Expected hidden output includes the region/event summary plus audio telemetry fields.

Route verification command:

- `SDL_AUDIODRIVER=dummy SDL_VIDEODRIVER=dummy RESONANCE_DEMO_HIDDEN=1 RESONANCE_DEMO_ROUTE=memory_chain build/resonance_demo`

Expected route output includes `RouteStep=meadow-start`, `RouteStep=station-stage-two`, `RouteStep=station-stage-three`, and ends with `Event=terminal_refrain`.

- `meadow` resolves at `(10, 10)`.
- `ruins` resolves at `(420, 10)`.
- Positions outside defined regions resolve to no active region.
- `AudioDirector` uses `explore` for `meadow` by default.
- A `mysterious` story cue overrides the current region default.
- `EventDirector` selects `old_broadcast_echo` for `ruins` because it has the highest weight there.

## Current Findings

- Core bootstrap, time, world, audio-state, event-selection, trace-log, story-focus, event-emphasis, and event-mix-profile slices are covered by automated tests.
- Demo content now includes 4 regions, 3 music states, 6 narrative events, and 4 story anchors.
- The audio backend now plays looped assets, applies story-focus shaping, adds a short event-emphasis fade when narrative events change, and consumes authored per-event mix profiles through the resolved runtime gains.

## Next Work

- Merge `World`, `AudioDirector`, `EventDirector`, and `DebugOverlay` into a single live demo loop.
- Add real `SDL3` rendering and visible debug text.
- Upgrade the audio backend from state tracking to actual playback.
