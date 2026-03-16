# Story Anchor Markers Design

## Goal

Make story anchors easier to discover in the SDL demo by rendering lightweight world-space markers and highlighting nearby / activated anchors.

## Chosen Approach

Expose a small visual state from `DemoScene` instead of mixing world queries directly into the renderer.

- `DemoScene` already owns story-anchor data and proximity state
- `main.cpp` should only draw what the scene exposes
- markers remain debug-friendly and deterministic

## Marker States

Each story anchor should expose:

- `id`
- `position`
- `is_nearby`
- `is_active`

Rendering policy:

- default anchor: muted cyan marker
- nearby anchor: warm gold marker
- activated anchor: bright white inner mark layered over the marker

## Why this approach

- keeps render logic simple
- gives tests a pure C++ surface to verify
- avoids coupling SDL drawing code to content lookup
- complements the existing overlay prompt instead of replacing it

## Out of Scope

- animated particles
- minimap
- discovery fog
- branching marker states driven by quests
