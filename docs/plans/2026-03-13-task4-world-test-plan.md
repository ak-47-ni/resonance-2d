# Task 4 World Traversal Test Plan

## Summary

- Feature/Change: Add a small world model with rectangular regions, player traversal, and current-region detection.
- Module/Area: `engine/world/World`, `engine/world/Trigger`, `game/demo/DemoScene`, `assets/data/regions/regions.json`.
- Scope: Region switching and a headless demo scene surface only.

## Test Mapping (1:1)

| Code Change | File/Function | Test Change | Test File |
| --- | --- | --- | --- |
| Add world traversal API | `engine/world/World.*` | Verify player movement across regions changes the active region id | `tests/world/test_region_switch.cpp` |
| Add rectangular trigger primitive | `engine/world/Trigger.*` | Exercise containment logic indirectly through the world traversal test | `tests/world/test_region_switch.cpp` |
| Add demo scene wrapper | `game/demo/DemoScene.*` | Rebuild demo target against the world API | `tests/world/test_region_switch.cpp` |
| Extend region data with bounds | `engine/world/RegionData.*`, `assets/data/regions/regions.json` | Use real region definitions in the region traversal test | `tests/world/test_region_switch.cpp` |

## Test Cases

- Happy path: moving the player between two non-overlapping regions updates `current_region_id()`.
- Edge cases: positions outside all regions produce an empty current region.
- Error cases: malformed region bounds fail during content loading.

## Test Execution

- Command(s): `cmake --build build && ctest --test-dir build -R region_switch --output-on-failure`
- Environment: local macOS shell with `cmake`, `ninja`, and the current SDL setup.
- Data fixtures: `assets/data/regions/regions.json`
- Expected duration: under 10 seconds.

## Risks

- Known flakiness: none.
- External dependencies: none beyond the existing project dependencies.
- Follow-up work: later tasks can reuse the world model for trigger-driven events.
