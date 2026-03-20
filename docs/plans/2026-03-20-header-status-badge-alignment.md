# 2026-03-20 Header Status Badge Alignment

## Goal
Align the header status strip with the newer viewport toolbar badge language so top-level shell status feels more intentional and less like raw debug labels placed into fixed-width slots.

## Scope
- `game/demo/WorldWorkspaceRenderModel.h`
- `game/demo/main.cpp`
- `tests/demo/test_demo_runtime.cpp`
- shared task board update only

## What Landed
- added `header_status_badges` to `WorldWorkspaceRenderModel`
- render-model now centralizes header status badge content as:
  - save badge
  - dirty badge
  - mode badge
- SDL shell now renders the header status strip with right-aligned dynamic-width badges instead of fixed-width direct draws from `view_model`
- header strip presentation is now visually aligned with the viewport toolbar badge compaction pass while preserving the same status semantics

## Validation
- `cmake --build build --target demo_runtime_test`
- `./build/demo_runtime_test`
- `cmake --build build --target resonance_demo demo_editor_save_test editor_document_test world_workspace_state_test`
- `ctest --test-dir build -R 'demo_runtime|demo_editor_save|world_workspace_state|editor_document' --output-on-failure`
- `SDL_AUDIODRIVER=dummy SDL_VIDEODRIVER=dummy RESONANCE_DEMO_HIDDEN=1 RESONANCE_DEMO_AUTOCLOSE=1 ./build/resonance_demo`

## Notes
- this remains a Terminal A shell/runtime integration slice
- no changes to `engine/audio/*`, persistence helpers, or `CMakeLists.txt`
