# Viewport Selection Feedback

## Goal

Make the `world_workspace` viewport communicate current editor focus more clearly without adding a new editor interaction model.

## What Changed

### Testable viewport chrome regions

Added dedicated layout rects for:

- `viewport_focus_chip`
- `viewport_hint_band`

This keeps the in-viewport feedback anchored to layout data instead of hardcoded pixel logic only.

### State-driven viewport feedback tokens

Added render-model fields for:

- `viewport_focus_label`
- `viewport_focus_detail`
- `viewport_hint_lines`

These now vary by context:

- world workspace with no selection
- anchor workspace with no selection
- region selection
- story anchor selection

### SDL viewport feedback

The viewport now renders:

- a focus chip near the top-left of the canvas
- a hint band near the bottom of the canvas
- a region label chip when a region is selected
- an anchor label chip with radius when a story anchor is selected

This improves readability without changing selection, drag, resize, or save behavior.

## Validation

Validated with:

- `ctest --test-dir build -R '^demo_runtime$' --output-on-failure`
- `ctest --test-dir build -R 'world_workspace_state|demo_runtime|demo_editor_save|editor_document' --output-on-failure`
- `SDL_AUDIODRIVER=dummy SDL_VIDEODRIVER=dummy RESONANCE_DEMO_HIDDEN=1 RESONANCE_DEMO_AUTOCLOSE=1 ./build/resonance_demo`

## Remaining Gaps

Still not done:

- hover feedback
- drag ghost / preview states
- stronger region resize handles
- dedicated viewport tools beyond text cues
- editor-quality scene gizmos
