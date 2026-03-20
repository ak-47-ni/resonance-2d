# Handle And Ghost Feedback

## Goal

Move one step closer to real editor interaction by showing when a resize/radius handle is hot and by adding lightweight drag ghost cues during active dragging.

## What Changed

### DemoScene now tracks gizmo-hot and drag-delta state

Added lightweight editor runtime state for:

- `hovered_gizmo_id`
- `editor_drag_delta`

This lets the shell distinguish plain selection from actual handle targeting and active drag motion.

### Workspace state/render model now expose ghost and hot-handle cues

Added state-driven cues for:

- hot region resize handle
- hot anchor radius handle
- ghost preview label/detail during drag

This keeps the shell copy in the same state-first pipeline instead of hardcoding mouse assumptions directly in drawing code.

### SDL viewport now renders hot handles and ghost trails

The viewport now shows:

- brighter handle fill when the region resize handle is hot
- brighter handle fill when the anchor radius handle is hot
- faint ghost rectangle for selected regions during drag
- faint ghost anchor marker during drag
- ghost text under the viewport focus chip

## Validation

Validated with:

- `ctest --test-dir build -R 'world_workspace_state|demo_runtime|demo_editor_save|editor_document' --output-on-failure`
- `SDL_AUDIODRIVER=dummy SDL_VIDEODRIVER=dummy RESONANCE_DEMO_HIDDEN=1 RESONANCE_DEMO_AUTOCLOSE=1 ./build/resonance_demo`

## Remaining Gaps

Still not done:

- true handle hit-driven editing paths instead of shared drag logic
- separate ghost preview before mutating real object state
- cursor/tool mode changes
- transform gizmo widgets
- panel-to-canvas cross-highlighting for more than the first inspector card
