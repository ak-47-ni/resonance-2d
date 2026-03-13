# Resonance 2D MVP Verification Checklist

## Automated Verification

Run from the repository root:

- `cmake -S . -B build -G Ninja`
- `cmake --build build`
- `ctest --test-dir build --output-on-failure`
- `python3 tools/validate_assets.py`

Expected results:

- Build completes successfully.
- All tests pass.
- Asset validation prints `Asset validation passed`.

## Manual Logic Verification

- `meadow` resolves at `(10, 10)`.
- `ruins` resolves at `(420, 10)`.
- Positions outside defined regions resolve to no active region.
- `AudioDirector` uses `explore` for `meadow` by default.
- A `mysterious` story cue overrides the current region default.
- `EventDirector` selects `old_broadcast_echo` for `ruins` because it has the highest weight there.

## Current Findings

- Core bootstrap, time, world, audio-state, event-selection, and trace-log slices are all covered by automated tests.
- Demo content now includes 4 regions, 3 music states, and 6 narrative events.
- The audio backend is currently a thin state-application shim; real playback remains future work.

## Next Work

- Merge `World`, `AudioDirector`, `EventDirector`, and `DebugOverlay` into a single live demo loop.
- Add real `SDL3` rendering and visible debug text.
- Upgrade the audio backend from state tracking to actual playback.
