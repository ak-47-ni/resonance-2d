# 2026-03-19 Mode + Tool Cues

## Goal
- Align the embedded `world_workspace` shell more closely with the prototype's `mode_status` and `canvas_tools_card` language.
- Keep the slice inside Terminal A's runtime/editor-shell boundary without touching editor persistence or audio backend modules.

## Prototype Alignment
From `docs/prototypes/resonance-editor-workspaces.fig` on the `world_workspace` page:
- `mode_status` keeps edit/play context visible in the header.
- `canvas_tools_card` communicates which editor tools are available or currently active.

## Landed
- Added explicit toolbar cues for current tool mode and cursor cue.
- Swapped the third world bottom card to `canvas_tools` so the shell now exposes tool availability instead of repeating save state in the dock.
- Kept save/dirty/mode surfaced in header badges and toolbar status.
- Added runtime logic that maps current interaction state into:
  - `tool:select`
  - `tool:move_region`
  - `tool:move_anchor`
  - `tool:resize_bounds`
  - `tool:adjust_radius`
- Added cursor cue mapping for:
  - `cursor:crosshair`
  - `cursor:pointer`
  - `cursor:move`
  - `cursor:grabbing`
  - `cursor:nwse-resize`
  - `cursor:ew-resize`
- Applied matching SDL system cursors in editor mode.

## Files
- `game/demo/WorldWorkspaceRenderModel.h`
- `game/demo/main.cpp`
- `game/demo/DemoScene.cpp`
- `tests/demo/test_demo_runtime.cpp`

## Verification
- `ctest --test-dir build -R 'demo_runtime|world_workspace_state|demo_editor_save|editor_document' --output-on-failure`
- `SDL_AUDIODRIVER=dummy SDL_VIDEODRIVER=dummy RESONANCE_DEMO_HIDDEN=1 RESONANCE_DEMO_AUTOCLOSE=1 ./build/resonance_demo`
