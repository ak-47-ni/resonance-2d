# Task 11 Audio Playback Test Plan

## Summary

- Feature/Change: Add deterministic tone-profile mapping and use it to drive a minimal SDL3 real-time audio backend.
- Module/Area: `engine/audio/AudioToneProfile`, `engine/audio/AudioBackendSDL`.
- Scope: Synthesized tone output for the current music state.

## Test Mapping (1:1)

| Code Change | File/Function | Test Change | Test File |
| --- | --- | --- | --- |
| Add tone profile resolver | `engine/audio/AudioToneProfile.*` | Verify known music states resolve to audible profiles and empty state resolves to silence | `tests/audio/test_audio_tone_profile.cpp` |
| Upgrade SDL backend to synthesize audio | `engine/audio/AudioBackendSDL.cpp` | Covered indirectly through the same deterministic profile mapping | `tests/audio/test_audio_tone_profile.cpp` |

## Test Cases

- Happy path: `mysterious` and `explore` resolve to different non-zero frequencies.
- Edge cases: empty state resolves to zero gain.
- Error cases: unknown state falls back to a safe default profile.

## Test Execution

- Command(s): `cmake --build build && ctest --test-dir build -R 'audio_tone_profile|audio_state' --output-on-failure`
- Environment: local macOS shell with SDL3 installed.
- Data fixtures: none.
- Expected duration: under 10 seconds.

## Risks

- Known flakiness: none.
- External dependencies: real audible output remains a manual check.
- Follow-up work: replace synthesized tones with decoded assets later.
