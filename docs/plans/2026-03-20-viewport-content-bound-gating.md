# 2026-03-20 Viewport Content-Bound Gating

## Goal
Tighten editor interaction boundaries so world editing only happens inside the viewport content area, not from header tabs, bottom dock, or inspector shell chrome.

## Scope
- `game/demo/DemoScene.*`
- `game/demo/main.cpp`
- `game/demo/WorldWorkspaceLayout.h`
- `tests/demo/test_demo_runtime.cpp`
- shared task board update only

## What Landed
- added `world_workspace_viewport_contains(...)` layout helper for screen-space viewport content hit testing
- added `DemoScene::clear_editor_hover()` so shell-side pointer movement can clear stale hover/gizmo state cleanly
- gated editor mouse interactions in `main.cpp` so these now require viewport-content hit:
  - middle-button pan activation
  - left-click selection / space+drag pan activation
  - hover updates when idle
  - wheel-driven zoom / selection primary adjustments
- preserved active drag continuity: once a drag is already active, motion can continue even if the pointer slips slightly outside the viewport
- shell clicks outside the viewport now clear hover state instead of accidentally selecting or manipulating world content

## Validation
- `cmake --build build --target demo_runtime_test`
- `./build/demo_runtime_test`
- `cmake --build build --target resonance_demo demo_editor_save_test editor_document_test world_workspace_state_test`
- `ctest --test-dir build -R 'demo_runtime|demo_editor_save|world_workspace_state|editor_document' --output-on-failure`
- `SDL_AUDIODRIVER=dummy SDL_VIDEODRIVER=dummy RESONANCE_DEMO_HIDDEN=1 RESONANCE_DEMO_AUTOCLOSE=1 ./build/resonance_demo`

## Notes
- no changes to `engine/audio/*`, persistence helpers, or `CMakeLists.txt`
- this slice is intentionally state-light: it tightens input routing without changing editor data flow
