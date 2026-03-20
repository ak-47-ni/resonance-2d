# 2026-03-19 Handle Drag Semantics

## Goal
- Promote `world_workspace` interactions from generic drag hints to handle-specific editing semantics.
- Keep Terminal A scoped to runtime shell and SDL affordances without colliding with editor persistence or audio backend work.

## Landed
- Added a persistent `active_gizmo_id` channel from `DemoScene` into `WorldWorkspaceState`.
- Locked drag semantics once a resize/radius handle is engaged so the interaction keeps its mode even if pointer hover leaves the old hotspot during the same drag.
- Routed drag behavior by mode:
  - `region_resize` drag now updates region bounds instead of moving the region.
  - `anchor_radius` drag now updates activation radius instead of moving the anchor.
  - generic drag still moves the selected region/anchor.
- Updated render-model copy for tool-specific drag states:
  - `Resizing Region`
  - `Adjusting Radius`
  - `Resize Preview`
  - `Radius Preview`
- Updated SDL ghost rendering so handle drags preview the previous bounds/radius rather than only a move ghost.
- Shifted idle inspector emphasis to the session card, while hover/selection keeps emphasis on the inspector card.

## Files
- `game/demo/DemoScene.h`
- `game/demo/DemoScene.cpp`
- `game/demo/main.cpp`
- `game/demo/WorldWorkspaceRenderModel.h`
- `engine/editor/WorldWorkspaceState.h`
- `engine/editor/WorldWorkspaceState.cpp`
- `tests/demo/test_demo_runtime.cpp`
- `tests/editor/test_world_workspace_state.cpp`

## Verification
- `ctest --test-dir build -R 'world_workspace_state|demo_runtime|demo_editor_save|editor_document' --output-on-failure`
- `SDL_AUDIODRIVER=dummy SDL_VIDEODRIVER=dummy RESONANCE_DEMO_HIDDEN=1 RESONANCE_DEMO_AUTOCLOSE=1 ./build/resonance_demo`
