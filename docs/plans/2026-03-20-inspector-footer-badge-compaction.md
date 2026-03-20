# 2026-03-20 Inspector Footer Badge Compaction

## Goal
Bring the right-side panel into the same badge + micro-summary language as the header, page hero, and viewport toolbar by compacting the inspector focus band and properties footer.

## Scope
- `game/demo/WorldWorkspaceRenderModel.h`
- `game/demo/main.cpp`
- `tests/demo/test_demo_runtime.cpp`
- shared task board update only

## What Landed
- added `inspector_focus_badges` to `WorldWorkspaceRenderModel`
- added `properties_footer_badges` to `WorldWorkspaceRenderModel`
- render-model now derives compact right-panel badges for:
  - inspector focus kind (`inspect:idle`, `inspect:region`, `inspect:anchor`)
  - inspector state (`state:track`, `state:hover`, `state:select`)
  - properties footer workspace (`workspace:world`, `workspace:anchor`)
  - properties footer selection (`selection:none`, `selection:region`, `selection:anchor`)
- SDL shell now renders these badges directly in the focus band and footer band while keeping the existing text lines as micro-summary details underneath
- tests cover default idle panel, hovered region panel, and selected-anchor panel

## Validation
- `cmake --build build --target demo_runtime_test`
- `./build/demo_runtime_test`
- `cmake --build build --target resonance_demo demo_editor_save_test editor_document_test world_workspace_state_test`
- `ctest --test-dir build -R 'demo_runtime|demo_editor_save|world_workspace_state|editor_document' --output-on-failure`
- `SDL_AUDIODRIVER=dummy SDL_VIDEODRIVER=dummy RESONANCE_DEMO_HIDDEN=1 RESONANCE_DEMO_AUTOCLOSE=1 ./build/resonance_demo`

## Notes
- this remains a Terminal A shell/runtime integration slice
- no changes to `engine/audio/*`, persistence helpers, or `CMakeLists.txt`
