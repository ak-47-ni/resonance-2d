# 2026-03-20 Viewport Toolbar Badge Compaction

## Goal
Compress the viewport toolbar into a more editor-like status strip by turning frame/tool/cursor context into compact badges instead of long raw debug strings.

## Scope
- `game/demo/WorldWorkspaceRenderModel.h`
- `game/demo/main.cpp`
- `tests/demo/test_demo_runtime.cpp`
- shared task board update only

## What Landed
- added `toolbar_context_badges` to `WorldWorkspaceRenderModel`
- render-model now derives three compact toolbar badges:
  - scope badge (`scope:view`, `scope:hover`, `scope:select`, `scope:preview`, `scope:nav`)
  - tool badge (`tool:select`, `tool:move`, `tool:resize`, `tool:radius`, `tool:navigate`)
  - cursor badge (`cursor:xhair`, `cursor:pointer`, `cursor:move`, `cursor:grab`, `cursor:nwse`, `cursor:ew`)
- SDL shell now renders right-aligned badges for save status + scope + tool + cursor in the viewport toolbar
- existing raw render-model fields remain intact, so behavior/tests stay state-first while visual presentation gets denser and cleaner

## Validation
- `cmake --build build --target demo_runtime_test`
- `./build/demo_runtime_test`
- `cmake --build build --target resonance_demo demo_editor_save_test editor_document_test world_workspace_state_test`
- `ctest --test-dir build -R 'demo_runtime|demo_editor_save|world_workspace_state|editor_document' --output-on-failure`
- `SDL_AUDIODRIVER=dummy SDL_VIDEODRIVER=dummy RESONANCE_DEMO_HIDDEN=1 RESONANCE_DEMO_AUTOCLOSE=1 ./build/resonance_demo`

## Notes
- this slice stays in Terminal A ownership: runtime/editor shell and main entry
- no changes to `engine/audio/*`, persistence helpers, or `CMakeLists.txt`
