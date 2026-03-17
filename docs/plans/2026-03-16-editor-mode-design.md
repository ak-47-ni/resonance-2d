# Editor Mode Design

## Goal

Add a first-generation in-engine editor mode to `resonance_demo` so the project can evolve from a runtime prototype into a lightweight engine with live content authoring.

## Why now

The runtime is already good enough to move around, preview narrative state, inspect debug telemetry, and validate content. The next leverage point is not more isolated runtime features, but a way to author world content in place.

The fastest path is not a separate Unreal-style application. It is an embedded SDL editor mode inside the existing demo.

## Chosen Direction

Build an **embedded SDL editor mode** inside `resonance_demo`.

The first version focuses on the content types that already exist and are highly visible in the current prototype:

- `regions`
- `story_anchors`

This keeps the work close to the current engine architecture:

- `World` already owns region geometry
- `DemoScene` already knows anchor positions and current player context
- JSON content already exists and is validated by tools
- the demo already renders debug overlays and accepts input

## Alternatives Considered

### Option A — Embedded SDL editor mode inside the current demo (recommended)

Pros:
- reuses the current runtime loop
- immediate visual feedback in the same world you play
- minimal startup cost
- easiest way to learn what editor abstractions the engine actually needs
- best fit for two-terminal parallel work right now

Cons:
- UI will be rough at first
- editor code initially lives close to demo/runtime code

### Option B — Separate desktop editor application

Pros:
- cleaner long-term separation
- easier to imagine a future full editor shell

Cons:
- duplicates rendering/input/bootstrap work too early
- slower to ship the first useful tool
- adds product surface before the data model is mature

### Option C — External JSON-only tool or web form editor

Pros:
- easiest standalone tooling
- can be implemented quickly

Cons:
- loses in-world spatial feedback
- weak fit for anchor placement and region sizing
- less aligned with your “engine editor” goal

## First Version Scope

The first editor version should support:

- toggle between `Play` and `Edit`
- mouse selection of a region or story anchor
- clear visual highlight for the selected object
- simple on-screen inspector/overlay showing editable fields
- direct manipulation of story anchor position
- radius adjustment for story anchors
- region bounds inspection, with region editing prepared but not necessarily complete in v1
- save current edited content back to JSON

### Story anchor fields in scope

- `id` (read-only in v1)
- `region_id` (view first, edit later if cheap)
- `x`
- `y`
- `activation_radius`
- `prompt_text`
- `story_text`

### Region fields in scope

- `id` (read-only in v1)
- `default_music_state` (view first)
- `bounds`

## Out of Scope for v1

- event graph editing
- timeline sequencing
- audio routing graph editing
- visual scripting
- multi-window docking UI
- undo/redo stack
- generalized asset browser
- plugin architecture

## Architecture Split

To keep Terminal A and Terminal B independent, split the work like this.

### Terminal A — Editor shell and in-scene interaction

Terminal A owns the runtime-facing editor mode:

- edit-mode toggle and mode state wiring
- scene hit testing hooks
- selection/highlight rendering
- drag/move interaction for story anchors
- save command integration into the current demo
- top-level editor overlay shell

This work naturally lives near:

- `game/demo/main.cpp`
- `game/demo/DemoScene.*`
- any new lightweight editor scene/controller files

### Terminal B — Editor data model, inspector state, and JSON writeback support

Terminal B owns data- and panel-centric editor logic:

- editor selection data structures
- inspector state and editable field abstractions
- serialization / writeback helpers for regions and story anchors
- validation-friendly save helpers
- tests for round-trip editing behavior

This work should stay as independent as possible from live SDL interaction.

## Recommended v1 Delivery Order

### Milestone 1

- add `Edit` mode
- select and highlight story anchors
- inspector shows anchor fields

### Milestone 2

- drag story anchors
- adjust radius
- save story anchors JSON

### Milestone 3

- inspect regions
- prepare or implement direct region bounds editing
- improve editor overlay readability

## Data Flow

1. user enters `Edit` mode
2. scene click resolves a selected object
3. selection is stored in editor state
4. inspector reads from editor state
5. edits mutate an in-memory content model
6. save command writes JSON back to `assets/data/...`
7. runtime refreshes scene state from the edited data or immediately reflects the in-memory state

## Testing Strategy

- unit tests for selection/serialization helpers
- round-trip tests for edited story anchor / region data
- smoke test that editor save paths preserve valid JSON structure
- manual demo verification for selection, dragging, and saving

## Success Criteria

The first editor version is successful if you can:

- launch the same demo executable
- switch into edit mode
- click a story anchor and see its details
- move it and adjust its radius
- save the change back to JSON
- relaunch and observe the edited content persist
