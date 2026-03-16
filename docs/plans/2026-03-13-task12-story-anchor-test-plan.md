# Task 12 Story Anchor Interaction Test Plan

## Summary

- Feature/Change: Add data-driven story anchors with proximity prompts and interaction text in the demo runtime.
- Module/Area: `engine/world/StoryAnchorData`, `game/demo/DemoScene`, `engine/debug/DebugOverlay`, `game/demo/main.cpp`.
- Scope: Content loading, interaction state, overlay rendering, and SDL key binding for `E`.

## Test Mapping (1:1)

| Code Change | File/Function | Test Change | Test File |
| --- | --- | --- | --- |
| Load story-anchor content | `engine/world/StoryAnchorData.*`, `engine/event/EventData.*` | Verify demo content bundle includes authored story anchors | `tests/data/test_story_anchor_content.cpp` |
| Resolve nearby anchor and activate story text | `game/demo/DemoScene.*` | Verify prompt appears near anchor and interaction activates story text | `tests/demo/test_story_anchor_interaction.cpp` |
| Render prompt/story lines in overlay | `engine/debug/DebugOverlay.*` | Verify overlay includes prompt and story lines | `tests/debug/test_overlay_panel.cpp` |
| Preserve demo runtime summary behavior while exposing interaction state | `game/demo/main.cpp`, `game/demo/DemoScene.*` | Verify demo overlay/state remains valid with story anchors present | `tests/demo/test_demo_runtime.cpp`, `tests/demo/test_demo_visual_state.cpp` |

## Test Cases

- Happy path: player moves near `meadow-swing`, overlay shows `Action: Press E to listen`, and interaction activates story text.
- Edge case: interacting when no anchor is nearby is a no-op.
- Edge case: anchor content references a valid region and positive radius.
- Error case: validator fails if a story anchor references a missing region or has non-positive radius.

## Test Execution

- Command(s): `cmake --build build && ctest --test-dir build -R 'story_anchor_content|story_anchor_interaction|overlay_panel|demo_runtime|demo_visual_state' --output-on-failure`
- Full verification: `cmake -S . -B build -G Ninja && cmake --build build && ctest --test-dir build --output-on-failure && python3 tools/validate_assets.py && SDL_AUDIODRIVER=dummy SDL_VIDEODRIVER=dummy RESONANCE_DEMO_HIDDEN=1 RESONANCE_DEMO_AUTOCLOSE=1 ./build/resonance_demo`
- Environment: local macOS shell with SDL3 available.
- Data fixtures: `assets/data/story/story_anchors.json`.

## Risks

- Known flakiness: none expected because interaction logic is deterministic.
- External dependencies: SDL input path is still primarily verified through C++ scene tests plus hidden demo smoke.
- Follow-up work: interaction markers, persistence, and richer branching narration.
