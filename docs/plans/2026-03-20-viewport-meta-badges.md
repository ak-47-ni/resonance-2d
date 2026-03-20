# 2026-03-20 Viewport Meta Badges

## Goal
Make the viewport feel more like a distinct editing canvas by upgrading the top-right canvas telemetry from a loose text line into compact shell badges aligned with the rest of the workspace chrome.

## Scope
- `game/demo/WorldWorkspaceRenderModel.h`
- `game/demo/main.cpp`
- `tests/demo/test_demo_runtime.cpp`
- shared task board update only

## What Landed
- added `viewport_meta_badges` to `WorldWorkspaceRenderModel`
- render-model now derives compact viewport telemetry badges from editor state:
  - `zoom:*`
  - `origin:*`
  - `pan:*`
- SDL shell now renders these badges in the viewport header instead of the old single `canvas_meta_label` text line
- kept the lower frame detail line so the canvas still exposes a softer narrative/status explanation under the tighter telemetry row
- extended runtime tests to cover idle, navigated, and pan-active viewport badge states

## Validation
- `cmake --build build --target demo_runtime_test`
- `./build/demo_runtime_test`
- `ctest --test-dir build -R 'demo_runtime|demo_editor_save|world_workspace_state|editor_document' --output-on-failure`
- `SDL_AUDIODRIVER=dummy SDL_VIDEODRIVER=dummy RESONANCE_DEMO_HIDDEN=1 RESONANCE_DEMO_AUTOCLOSE=1 ./build/resonance_demo`

## Notes
- this remains a Terminal A shell/runtime integration slice
- no changes to `engine/audio/*`, `engine/editor/*`, persistence helpers, or `CMakeLists.txt`
