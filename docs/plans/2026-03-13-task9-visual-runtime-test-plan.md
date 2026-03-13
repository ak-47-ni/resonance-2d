# Task 9 Visual Runtime Test Plan

## Summary

- Feature/Change: Expose render-facing scene state and upgrade the demo executable to a visible SDL3 loop.
- Module/Area: `game/demo/DemoScene`, `game/demo/main.cpp`, `engine/world/World`.
- Scope: Visible region/player rendering and title-based debug state.

## Test Mapping (1:1)

| Code Change | File/Function | Test Change | Test File |
| --- | --- | --- | --- |
| Expose render-facing scene state | `game/demo/DemoScene.*`, `engine/world/World.*` | Verify the scene reports region count, player position, and a formatted debug summary | `tests/demo/test_demo_visual_state.cpp` |
| Upgrade demo runtime to SDL3 loop | `game/demo/main.cpp` | Keep demo logic buildable against the new scene surface | `tests/demo/test_demo_visual_state.cpp` |

## Test Cases

- Happy path: `meadow` update exposes 4 regions, player position `(10, 10)`, and a debug summary containing region, music, and event.
- Edge cases: unknown space clears the summary event field.
- Error cases: empty overlay state still formats a valid summary string.

## Test Execution

- Command(s): `cmake --build build && ctest --test-dir build -R 'demo_visual_state|demo_runtime' --output-on-failure`
- Environment: local macOS shell with SDL3 installed.
- Data fixtures: `assets/data/**/*.json`
- Expected duration: under 10 seconds.

## Risks

- Known flakiness: none.
- External dependencies: visible rendering itself remains a manual check.
- Follow-up work: draw text on-screen instead of only in the window title.
