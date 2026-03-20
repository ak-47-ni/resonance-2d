# Inspector And Gizmo Feedback

## Goal

Add a little more editor readability to the `world_workspace` shell by making the properties side react to hover/selection context and by surfacing lightweight gizmo hints inside the viewport.

## What Changed

### Layout now has dedicated focus/gizmo slots

Added dedicated rects for:

- `properties_focus_band`
- `viewport_gizmo_badge`

This keeps the new chrome anchored in layout data instead of ad-hoc coordinates only.

### Render model now exposes inspector and gizmo cues

Added state-driven fields for:

- `inspector_focus_label`
- `inspector_focus_detail`
- `inspector_emphasis_index`
- `viewport_gizmo_label`
- `viewport_gizmo_detail`

These react to idle, hover, selected, and dragging states.

### SDL shell now surfaces those cues

The shell now renders:

- a focus band at the top of the properties panel
- stronger stroke emphasis on the active inspector card
- a small gizmo badge inside the viewport
- a short gizmo detail line under the badge

## Validation

Validated with:

- `ctest --test-dir build -R 'world_workspace_state|demo_runtime|demo_editor_save|editor_document' --output-on-failure`
- `SDL_AUDIODRIVER=dummy SDL_VIDEODRIVER=dummy RESONANCE_DEMO_HIDDEN=1 RESONANCE_DEMO_AUTOCLOSE=1 ./build/resonance_demo`

## Remaining Gaps

Still not done:

- true transform gizmos
- dedicated resize handle hit-testing
- pointer cursor / tool mode changes
- drag ghost preview separate from live mutation
- panel-level hover cross-highlighting beyond the first inspector card
