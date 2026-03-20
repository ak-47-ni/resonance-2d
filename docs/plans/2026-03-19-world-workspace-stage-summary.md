# World Workspace Stage Summary

## Goal

Close the current `world_workspace` shell stage at a point where the embedded editor has a minimal but coherent multi-workspace UI inside `resonance_demo`.

This stage does **not** aim to become a full editor yet. It aims to make the shell readable, state-driven, and usable enough to support the next iteration.

## What This Stage Now Supports

### Embedded editor shell inside the demo

The editor now lives inside the same SDL executable as the runtime demo.

The shell includes:

- header title, subtitle, mode badge, dirty badge, and save badge
- clickable header workspace tabs
- left rail session and navigation labels
- viewport toolbar labels and action chips
- state-driven footer and properties footer
- workspace-specific bottom cards and inspector cards

### Minimal multi-workspace switching

Two top-level workspaces now exist in the shell:

- `world_workspace`
- `anchor_workspace`

The shell can switch between them through keyboard shortcuts and header tab clicks.

### State-first shell rendering

The main shell reads from the editor workspace state pipeline instead of hardcoded label arrays.

Current flow:

- `WorldWorkspaceState`
- `WorldWorkspaceViewModel`
- `WorldWorkspaceRenderModel`
- SDL shell rendering in `game/demo/main.cpp`

This keeps more UI copy and panel structure out of the drawing code and makes future editor growth safer.

### Anchor workspace visual distinction

The anchor-focused workspace now has distinct shell copy instead of only reusing world labels.

Examples:

- workspace overlay switches to `Anchor Canvas`
- bottom cards switch to anchor-oriented titles such as `anchor_overview`, `anchor_audio`, and `anchor_status`
- inspector cards switch to anchor-oriented titles such as `anchor_selection`, `runtime_context`, and `session_status`

## Current Controls

### Mode and workspace

- `Tab` enters or leaves editor mode
- `1` switches to `world_workspace`
- `2` switches to `anchor_workspace`
- clicking the header tabs also switches workspace

### Existing runtime/editor controls kept alive

- drag inside editor view to pan
- `W`, `A`, `S`, `D` move the camera/player depending on current mode path
- mouse wheel zooms the viewport
- `[` and `]` adjust anchor radius in the current editor flow
- `F5` triggers save in the current editor path

## Validation Completed

This stage was validated with focused build, tests, and a hidden smoke run.

Validated commands:

- `cmake --build build --target world_workspace_state_test demo_runtime_test demo_editor_save_test editor_document_test resonance_demo`
- `ctest --test-dir build -R 'world_workspace_state|demo_runtime|demo_editor_save|editor_document' --output-on-failure`
- `SDL_AUDIODRIVER=dummy SDL_VIDEODRIVER=dummy RESONANCE_DEMO_HIDDEN=1 RESONANCE_DEMO_AUTOCLOSE=1 ./build/resonance_demo`

## What Still Remains

The project is now at a **minimal multi-workspace editor shell** stage, not a full editor stage.

Major next-step gaps are:

- real viewport composition improvements for world/anchor workspaces
- stronger visual selection and spatial editing affordances
- richer inspector editing instead of mostly structured readouts
- resize, split, and layout behavior for editor panels
- editor-quality interaction polish such as focus, hover, and drag feedback
- undo/redo and more explicit save lifecycle behavior

## Recommended Next Stage

The next best step is to build a more convincing `world_workspace` viewport and shell hierarchy on top of this state-first base.

Recommended focus:

1. improve workspace visual hierarchy and spacing
2. strengthen viewport boundaries and panel readability
3. make selection and anchor editing more obvious in-scene
4. prepare a slightly more editor-like interaction model before adding larger feature scope
