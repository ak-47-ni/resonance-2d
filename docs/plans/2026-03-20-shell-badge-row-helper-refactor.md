# 2026-03-20 Shell Badge Row Helper Refactor

## Goal
Reduce repeated badge-drawing code across the world workspace shell so header, hero, toolbar, inspector, and footer stay visually aligned and easier to evolve together.

## Scope
- `game/demo/main.cpp`
- shared task board update only

## What Landed
- introduced a small shared badge-row drawing layer in `main.cpp`:
  - `BadgeRowItem`
  - `draw_badge_row_left(...)`
  - `draw_badge_row_right(...)`
- widened `draw_workspace_badge(...)` to accept `std::string_view` so shell badge sources can pass through render-model strings without extra one-off conversions
- replaced duplicated inline badge loops with helper-based rendering for:
  - header status badges
  - page hero context badges
  - viewport toolbar context badges
  - inspector focus badges
  - properties footer badges
- preserved the existing render-model contract and focused test assertions while tightening the shell-side layout logic into one reusable path

## Validation
- `cmake --build build --target demo_runtime_test`
- `./build/demo_runtime_test`
- `ctest --test-dir build -R 'demo_runtime|demo_editor_save|world_workspace_state|editor_document' --output-on-failure`
- `SDL_AUDIODRIVER=dummy SDL_VIDEODRIVER=dummy RESONANCE_DEMO_HIDDEN=1 RESONANCE_DEMO_AUTOCLOSE=1 ./build/resonance_demo`

## Notes
- this remains a Terminal A shell/runtime integration slice
- no changes to `engine/audio/*`, persistence helpers, `engine/editor/*`, or `CMakeLists.txt`
