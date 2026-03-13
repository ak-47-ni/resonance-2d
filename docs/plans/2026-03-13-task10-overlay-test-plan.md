# Task 10 Overlay Panel Test Plan

## Summary

- Feature/Change: Add measurable on-screen debug overlay panel sizing and draw it through SDL3 debug text.
- Module/Area: `engine/debug/DebugOverlay`, `game/demo/main.cpp`.
- Scope: Panel metrics and on-screen rendering path.

## Test Mapping (1:1)

| Code Change | File/Function | Test Change | Test File |
| --- | --- | --- | --- |
| Add overlay panel metrics | `engine/debug/DebugOverlay.*` | Verify measured panel width and height reflect line content and count | `tests/debug/test_overlay_panel.cpp` |
| Render overlay in demo | `game/demo/main.cpp` | Covered indirectly by demo executable verification in hidden SDL mode | `tests/debug/test_overlay_panel.cpp` |

## Test Cases

- Happy path: three overlay lines produce positive width and height.
- Edge cases: empty line set still yields a padded panel footprint.
- Error cases: empty strings are handled without crashing.

## Test Execution

- Command(s): `cmake --build build && ctest --test-dir build -R 'overlay_panel|demo_visual_state' --output-on-failure`
- Environment: local macOS shell with SDL3 installed.
- Data fixtures: none.
- Expected duration: under 10 seconds.

## Risks

- Known flakiness: none.
- External dependencies: final visible rendering remains a manual check.
- Follow-up work: richer text rendering can replace the debug text API later.
