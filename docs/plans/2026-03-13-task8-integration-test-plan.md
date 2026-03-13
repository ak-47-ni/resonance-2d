# Task 8 Runtime Integration Test Plan

## Summary

- Feature/Change: Integrate `World`, `EventDirector`, `AudioDirector`, and `DebugOverlay` into a single demo runtime flow.
- Module/Area: `game/demo/DemoScene`, `game/demo/main.cpp`.
- Scope: Headless integration only; rendering remains future work.

## Test Mapping (1:1)

| Code Change | File/Function | Test Change | Test File |
| --- | --- | --- | --- |
| Wire demo runtime update loop | `game/demo/DemoScene.*` | Verify a scene update resolves region, event, music, and overlay text together | `tests/demo/test_demo_runtime.cpp` |
| Upgrade demo entry point | `game/demo/main.cpp` | Build the demo target against the integrated scene surface | `tests/demo/test_demo_runtime.cpp` |

## Test Cases

- Happy path: updating in `meadow` resolves the region, picks `distant_bell`, and pushes `mysterious` music.
- Edge cases: moving outside all regions clears the active event.
- Error cases: empty overlay sections still format without crashing.

## Test Execution

- Command(s): `cmake --build build && ctest --test-dir build -R demo_runtime --output-on-failure`
- Environment: local macOS shell.
- Data fixtures: `assets/data/**/*.json`
- Expected duration: under 10 seconds.

## Risks

- Known flakiness: none.
- External dependencies: none.
- Follow-up work: swap the headless integration loop into a visible SDL runtime.
