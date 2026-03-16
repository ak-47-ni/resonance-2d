# Multi-Memory Events Design

## Goal

Prove that event gating supports true multi-memory conditions, not just a single discovered memory.

## First Slice

Add one station event that requires both:

- `memory:meadow-swing`
- `memory:ruins-gate`

This event should remain unavailable when only one memory is discovered, and become available once both are present.

## Why this slice

- validates the all-tags-satisfied semantics already present in `required_world_tags`
- deepens the exploration loop without adding new systems
- stays deterministic and easy to test
