# World Workspace Visual Polish

## Goal

Improve the perceived quality of the embedded editor shell for `world_workspace` without changing runtime behavior or colliding with editor-side data work.

This slice focuses on shell hierarchy, viewport boundaries, and workspace-specific chrome cues.

## Boundary

In scope:

- `game/demo/WorldWorkspaceLayout.h`
- `game/demo/WorldWorkspaceRenderModel.h`
- `game/demo/main.cpp`
- focused runtime tests

Out of scope:

- `engine/audio/*`
- editor-side persistence schema changes
- new interaction modes
- content JSON updates

## Prototype Cues Pulled In

From the `world_workspace` prototype, this slice explicitly mirrors these ideas:

- grouped header tabs instead of tabs floating in empty space
- grouped status badges on the right side of the header
- a stronger viewport container with clearer chrome separation
- a visible canvas badge inside the viewport toolbar
- a more intentional footer treatment in the properties panel
- workspace-specific accent tone so `world_workspace` and `anchor_workspace` feel different at a glance

## Implementation Summary

### Layout-level shell boundaries

Added dedicated layout rects for:

- `header_tabs_strip`
- `header_status_strip`
- `viewport_scope_badge`
- `properties_footer_band`

These make the visual grouping testable instead of leaving them as ad-hoc coordinates inside drawing code.

### Render-model chrome tokens

Added state-driven chrome fields to the render model:

- `workspace_tone`
- `viewport_badge`

Current mapping:

- `world_workspace` -> `world` + `World Canvas`
- `anchor_workspace` -> `anchor` + `Anchor Canvas`

This keeps visual identity tied to editor workspace state instead of hardcoded branching in multiple drawing sites.

### SDL shell updates

Updated the shell rendering to use the new layout/model data:

- header tab strip and status strip get dedicated backplates
- active tab uses workspace accent color
- active rail item uses workspace accent surface/stroke
- primary action badge uses workspace accent color
- viewport frame uses accent-aware stroke treatment
- viewport toolbar now renders an explicit canvas badge
- properties panel footer now renders inside a dedicated footer band

## Validation

Validated with:

- `ctest --test-dir build -R '^demo_runtime$' --output-on-failure`
- `ctest --test-dir build -R 'world_workspace_state|demo_runtime|demo_editor_save|editor_document' --output-on-failure`
- `SDL_AUDIODRIVER=dummy SDL_VIDEODRIVER=dummy RESONANCE_DEMO_HIDDEN=1 RESONANCE_DEMO_AUTOCLOSE=1 ./build/resonance_demo`

## Result

The editor is still minimal, but the shell now reads more like a designed tool surface than a debug overlay with boxes. It remains state-first and keeps Terminal A work localized to runtime/mainline files.
