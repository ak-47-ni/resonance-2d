# Memory Journal Design

## Goal

Let the player retain discovered story fragments as a lightweight exploration journal, so story anchors feel cumulative instead of disposable.

## Chosen Approach

Add a small memory-journal state to `DemoScene` and a journal overlay mode in `DebugOverlay`.

- story anchors remain the source of authored memory content
- interacting with an anchor records it once into a discovered-memory list
- the player can toggle a journal view without leaving the runtime loop

## Why this approach

- minimal new state, no save system required
- works with the existing overlay and SDL input flow
- reinforces exploration without expanding world or audio systems

## Journal Behavior

- first interaction with a story anchor records a journal entry
- repeat interaction with the same anchor does not duplicate entries
- pressing `J` toggles overlay between runtime HUD and journal view
- journal view shows entry count and discovered memories in discovery order

## Data Shape

Each memory entry should contain:

- `id`
- `region_id`
- `story_text`

## Out of Scope

- persistence across sessions
- categories, tags, or search
- branching codex / encyclopedia UI
- voice playback
