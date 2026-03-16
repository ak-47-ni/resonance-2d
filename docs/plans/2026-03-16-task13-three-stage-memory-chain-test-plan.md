# Task 13 Three-Stage Memory Chain Test Plan

## Summary

- Feature/Change: Extend memory-driven events into a true three-stage chain with a final three-memory station event.
- Module/Area: `engine/event/EventData`, `engine/event/EventDirector`, `game/demo/DemoScene`, `assets/data/events/events.json`.
- Scope: multi-tag gating and runtime propagation of three discovered memories.

## Test Mapping (1:1)

| Code Change | File/Function | Test Change | Test File |
| --- | --- | --- | --- |
| Add final three-memory gated event | `assets/data/events/events.json` | Verify director returns `terminal_refrain` only when all three tags are present | `tests/event/test_event_director.cpp` |
| Propagate third discovered memory into runtime event selection | `game/demo/DemoScene.cpp` | Verify station event upgrades from stage 2 to stage 3 after discovering the lakeside memory | `tests/demo/test_three_stage_memory_chain.cpp` |

## Test Cases

- Happy path: three memory tags in context unlock `terminal_refrain`.
- Edge case: with only two memory tags, station still resolves to `platform_convergence`.
- Edge case: runtime discovers meadow, ruins, and lakeside memories in sequence, then station resolves to `terminal_refrain`.

## Test Execution

- Command(s): `cmake --build build && ctest --test-dir build -R 'event_director|three_stage_memory_chain' --output-on-failure`
- Full verification: `cmake -S . -B build -G Ninja && cmake --build build && ctest --test-dir build --output-on-failure && python3 tools/validate_assets.py && SDL_AUDIODRIVER=dummy SDL_VIDEODRIVER=dummy RESONANCE_DEMO_HIDDEN=1 RESONANCE_DEMO_AUTOCLOSE=1 ./build/resonance_demo`

## Risks

- Known flakiness: none expected because event gating is deterministic.
- Follow-up work: branching chains, stronger journal/event UI coupling, and audio-linked narrative stages.
