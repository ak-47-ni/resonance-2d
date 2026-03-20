# Viewport Interaction Feedback

## Goal

Make the embedded editor viewport feel more interactive by exposing hover, drag-state, and simple adjustment handles using the existing world/anchor editing flow.

## What Changed

### DemoScene now tracks live editor hover and drag state

Added lightweight editor runtime state for:

- hovered region id
- hovered story anchor id
- drag-active flag

This state stays in `DemoScene` and feeds the existing workspace state pipeline.

### Workspace state now carries viewport interaction summary

`WorldWorkspaceRuntimeSummary` and `WorldWorkspaceState` now include hover and drag fields so the render model can derive viewport copy without hardcoding scene logic inside the shell renderer.

### Viewport focus copy is now interaction-aware

The viewport focus chip and hint band now react to:

- idle world workspace
- idle anchor workspace
- hovered region
- hovered anchor
- selected region
- selected anchor
- dragging region
- dragging anchor

### Canvas now shows simple affordances

The viewport now renders:

- faint hover outline for hovered regions
- faint hover frame for hovered anchors
- bottom-right region resize handle for selected regions
- radius handle for selected anchors
- drag-aware focus chip and hint text during left-drag editing

## Validation

Validated with:

- `ctest --test-dir build -R '^demo_runtime$' --output-on-failure`
- `ctest --test-dir build -R 'world_workspace_state|demo_runtime|demo_editor_save|editor_document' --output-on-failure`
- `SDL_AUDIODRIVER=dummy SDL_VIDEODRIVER=dummy RESONANCE_DEMO_HIDDEN=1 RESONANCE_DEMO_AUTOCLOSE=1 ./build/resonance_demo`

## Remaining Gaps

Still not done:

- true drag ghost / delta preview separate from live object movement
- explicit resize hit-testing on handles
- hover-aware inspector emphasis
- scene gizmos and transform widgets
- editor cursor modes and dedicated tool switching
