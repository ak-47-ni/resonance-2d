# Editor Mode Terminal B Plan

> Terminal B scope: editor data model, inspector state, JSON writeback helpers, and non-SDL-heavy tests.

## Goal

Provide the content-editing backbone that Terminal A’s in-scene editor mode can use safely.

## Files likely owned by Terminal B

- new lightweight editor model files under `engine/` or `game/demo/` (to be chosen by Terminal B)
- story anchor / region serialization helpers
- save/load round-trip tests
- inspector state abstractions
- docs for the writeback path if useful

## Deliverables

### Task B1 — Editor selection / inspector model

- define types for selected object identity
- define inspector-friendly editable fields for story anchors and regions
- keep SDL/rendering concerns out of these types

### Task B2 — Writeback helpers

- load editable story anchor / region content into mutable in-memory structures
- write modified content back to JSON in a stable format
- preserve existing field meanings and validation compatibility

### Task B3 — Round-trip tests

- modify an anchor position/radius in memory and write it out
- reload the written JSON and verify values persist
- if region editing is included, add the same round-trip test for region bounds

### Task B4 — Save contract for Terminal A

- expose one small API Terminal A can call from editor mode
- document expected call flow and ownership boundaries in the shared board or docs

## Constraints

- avoid taking ownership of `game/demo/main.cpp` or live SDL event handling
- prefer isolated helpers and tests
- preserve compatibility with `tools/validate_assets.py`

## Verification

- writeback helpers produce valid JSON
- edited data reloads correctly
- existing validation still passes after save operations
- Terminal A can call a stable save/update surface without knowing JSON details
