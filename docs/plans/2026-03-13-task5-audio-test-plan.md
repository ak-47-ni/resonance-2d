# Task 5 Audio Director Test Plan

## Summary

- Feature/Change: Add an `AudioDirector` that resolves music state from region defaults and event-driven story cues, plus a minimal SDL-oriented backend interface.
- Module/Area: `engine/audio/AudioDirector`, `engine/audio/AudioBackend`, `engine/audio/AudioBackendSDL`.
- Scope: State resolution and backend plumbing only; no real audio mixing yet.

## Test Mapping (1:1)

| Code Change | File/Function | Test Change | Test File |
| --- | --- | --- | --- |
| Add music state resolver | `engine/audio/AudioDirector.*` | Verify region entry and cue requests resolve the expected current music state | `tests/audio/test_audio_state.cpp` |
| Add backend abstraction | `engine/audio/AudioBackend.*`, `engine/audio/AudioBackendSDL.cpp` | Exercise backend application indirectly through the director's resolved state path | `tests/audio/test_audio_state.cpp` |

## Test Cases

- Happy path: a story cue overrides the region default music state.
- Edge cases: entering a region with no mapping leaves the current state unchanged.
- Error cases: requesting an unknown story cue is ignored.

## Test Execution

- Command(s): `cmake --build build && ctest --test-dir build -R audio_state --output-on-failure`
- Environment: local macOS shell with the current SDL3 install.
- Data fixtures: none.
- Expected duration: under 10 seconds.

## Risks

- Known flakiness: none.
- External dependencies: `SDL3_mixer` is not available in Homebrew here, so the backend remains a thin SDL3-friendly stub for now.
- Follow-up work: bind resolved states to real music/ambient playback later.
