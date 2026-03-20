# 2026-03-19 Viewport Navigation Skeleton

## Goal
Add a real editor viewport navigation baseline for Terminal A's mainline shell: pan/zoom state, state-first propagation, runtime shell feedback, and real world/screen transform hookup.

## Scope
- `game/demo/DemoScene.*`
- `game/demo/main.cpp`
- `game/demo/WorldWorkspaceLayout.h`
- `game/demo/WorldWorkspaceRenderModel.h`
- `engine/editor/WorldWorkspaceState.*`
- focused runtime/editor tests only

## Controls
- `Shift + WASD` or `Shift + Arrow` : pan editor viewport
- `Middle Mouse Drag` : pan editor viewport directly
- `Space + Left Drag` : temporary hand-pan in editor viewport
- `-` / `=` / keypad plus-minus : zoom editor viewport
- `Ctrl + Mouse Wheel` : zoom editor viewport
- `F` : reset viewport origin + zoom back to default
- plain drag / wheel / bracket controls keep their current selection editing behavior

## What Landed
- `DemoScene` now owns `editor_viewport_origin` and `editor_viewport_zoom`
- workspace runtime state carries viewport origin/zoom for state-first rendering
- editor overlay exposes `Viewport Zoom`, `Viewport Origin`, and `Viewport Pan`
- `canvas_tools` card now surfaces `view_zoom`, `view_origin`, and `view_pan`
- world/screen conversion helpers are testable in `WorldWorkspaceLayout.h`
- editor drawing and hit-testing now apply the same viewport transform
- state-first pan-active cue drives `tool:navigate_view`, `cursor:grabbing`, and viewport guidance while panning

## Validation
- `ctest --test-dir build -R 'demo_runtime|demo_editor_save|world_workspace_state|editor_document' --output-on-failure`
- `SDL_AUDIODRIVER=dummy SDL_VIDEODRIVER=dummy RESONANCE_DEMO_HIDDEN=1 RESONANCE_DEMO_AUTOCLOSE=1 ./build/resonance_demo`

## Follow-up Ideas
- mouse middle-button / space-drag panning
- zoom-to-selection / reset viewport shortcut
- clip-aware selection marquee inside transformed viewport
