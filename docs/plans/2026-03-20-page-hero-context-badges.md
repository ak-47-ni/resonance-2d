# 2026-03-20 Page Hero Context Badges

## Goal
Unify the page hero area with the newer shell badge language by surfacing compact context badges next to the page title, so workspace/frame/selection state reads as a quick summary instead of being spread across multiple shell zones.

## Scope
- `game/demo/WorldWorkspaceRenderModel.h`
- `game/demo/main.cpp`
- `tests/demo/test_demo_runtime.cpp`
- shared task board update only

## What Landed
- added `page_context_badges` to `WorldWorkspaceRenderModel`
- render-model now derives three compact page-hero badges:
  - workspace badge (`workspace:world`, `workspace:anchor`)
  - frame badge (`frame:view`, `frame:hover`, `frame:select`, `frame:preview`, `frame:nav`)
  - selection badge (`selection:none`, `selection:region`, `selection:anchor`)
- SDL shell now draws these badges inline in the page hero row between the title block and action badges
- tests cover default world workspace, anchor workspace, and selected-anchor hero state

## Validation
- `cmake --build build --target demo_runtime_test`
- `./build/demo_runtime_test`
- `cmake --build build --target resonance_demo demo_editor_save_test editor_document_test world_workspace_state_test`
- `ctest --test-dir build -R 'demo_runtime|demo_editor_save|world_workspace_state|editor_document' --output-on-failure`
- `SDL_AUDIODRIVER=dummy SDL_VIDEODRIVER=dummy RESONANCE_DEMO_HIDDEN=1 RESONANCE_DEMO_AUTOCLOSE=1 ./build/resonance_demo`

## Notes
- this remains a Terminal A shell/runtime integration slice
- no changes to `engine/audio/*`, persistence helpers, or `CMakeLists.txt`
