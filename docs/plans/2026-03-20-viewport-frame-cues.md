# 2026-03-20 Viewport Frame Cues

## Goal
Make the editable viewport feel more obviously separate from shell chrome by exposing a small render-model state for frame emphasis and consuming it in the SDL shell as a stronger boundary cue.

## Scope
- `game/demo/WorldWorkspaceRenderModel.h`
- `game/demo/main.cpp`
- `tests/demo/test_demo_runtime.cpp`
- shared task board update only

## What Landed
- added `viewport_frame_state` and `viewport_frame_detail` to `WorldWorkspaceRenderModel`
- added state-first frame cue derivation for key modes:
  - `frame:viewport_only`
  - `frame:hover_focus`
  - `frame:selection_focus`
  - `frame:edit_preview`
  - `frame:navigate_view`
- `canvas_tools` now surfaces `view_scope: ...` so the shell exposes current canvas framing/interaction mode in a testable way
- SDL shell now consumes frame cues to:
  - tint the viewport inset border by current interaction mode
  - draw corner brackets on the active edit canvas
  - show a second line of canvas meta with current frame detail

## Validation
- `cmake --build build --target demo_runtime_test`
- `./build/demo_runtime_test`
- `cmake --build build --target resonance_demo demo_editor_save_test editor_document_test world_workspace_state_test`
- `ctest --test-dir build -R 'demo_runtime|demo_editor_save|world_workspace_state|editor_document' --output-on-failure`
- `SDL_AUDIODRIVER=dummy SDL_VIDEODRIVER=dummy RESONANCE_DEMO_HIDDEN=1 RESONANCE_DEMO_AUTOCLOSE=1 ./build/resonance_demo`

## Notes
- this slice stays in Terminal A ownership: runtime/editor shell and main entry
- no changes to `engine/audio/*`, persistence helpers, or `CMakeLists.txt`
