# Editor Mode Terminal A Plan

> Terminal A scope: editor shell, scene interaction, selection, visual feedback, and save command wiring.

## Goal

Turn `resonance_demo` into an executable that can switch between play mode and a first editor mode for world content.

## Files likely owned by Terminal A

- `game/demo/main.cpp`
- `game/demo/DemoScene.h`
- `game/demo/DemoScene.cpp`
- `engine/debug/DebugOverlay.h`
- `engine/debug/DebugOverlay.cpp`
- possible new files under `game/demo/` for editor shell/runtime interaction
- docs updates only if needed for Terminal A behavior

## Deliverables

### Task A1 — Edit mode shell

- add `Play` / `Edit` mode state
- keyboard shortcut to toggle mode
- editor mode banner/overlay

### Task A2 — Selection and highlight

- support clicking/selecting a story anchor in scene view
- render selected highlight separate from existing nearby/active visuals
- expose current selection to overlay

### Task A3 — Direct manipulation

- drag selected story anchor in world space
- support radius adjustment via simple keybind or wheel input
- keep interaction deterministic and debug-friendly

### Task A4 — Save trigger integration

- add a save shortcut in editor mode
- call into Terminal B-owned writeback helper instead of writing JSON directly in scene code if possible

## Constraints

- avoid owning JSON schema or writeback logic if Terminal B can provide a helper
- keep editor shell incremental and reversible
- do not turn this into a general UI framework yet

## Verification

- demo launches normally in play mode
- edit mode toggles cleanly
- selected story anchor can be moved and visually confirmed
- save hook is callable and does not crash the runtime
