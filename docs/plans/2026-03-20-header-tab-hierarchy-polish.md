# 2026-03-20 Header Tab Hierarchy Polish

## Goal
Bring the header workspace tabs closer to the `world_workspace` prototype by keeping only the active workspace as a filled pill and demoting inactive tabs into lighter text-level navigation.

## Scope
- `game/demo/WorldWorkspaceLayout.h`
- `game/demo/main.cpp`
- `tests/demo/test_demo_runtime.cpp`
- shared task board update only

## What Landed
- tightened the header tab layout to prototype-aligned positions:
  - active `World` pill remains at `476,16` with size `56x24`
  - inactive tab anchors now align to `544`, `612`, and `666`
- refined header tab rendering so:
  - active tab keeps the accent pill treatment
  - inactive tabs render as muted text instead of full card buttons
- extended runtime layout verification to assert the prototype-aligned header tab strip geometry and tab anchor positions

## Validation
- `cmake --build build --target demo_runtime_test`
- `./build/demo_runtime_test`
- `ctest --test-dir build -R 'demo_runtime|demo_editor_save|world_workspace_state|editor_document' --output-on-failure`
- `SDL_AUDIODRIVER=dummy SDL_VIDEODRIVER=dummy RESONANCE_DEMO_HIDDEN=1 RESONANCE_DEMO_AUTOCLOSE=1 ./build/resonance_demo`

## Notes
- this remains a Terminal A shell/runtime integration slice
- no changes to `engine/audio/*`, `engine/editor/*`, persistence helpers, or `CMakeLists.txt`
