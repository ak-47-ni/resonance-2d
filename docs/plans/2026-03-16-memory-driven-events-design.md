# Memory-Driven Events Design

## Goal

Let discovered memories change what random events can happen, so exploration leaves systemic traces in the world.

## Chosen Approach

Gate selected events behind `world_tags` derived from discovered journal entries.

- `EventData` gains optional `required_world_tags`
- `DemoScene` converts discovered memory entries into runtime tags like `memory:meadow-swing`
- `EventDirector` only considers gated events when all required tags are present

## Why this approach

- reuses the existing `EventContext.world_tags` hook
- keeps gating logic inside the event layer
- lets story anchors and random events reinforce each other
- preserves deterministic testability

## First Slice

Implement one guaranteed unlock path first:

- discover `meadow-swing`
- unlock a new meadow event: `swing_memory_echo`

That gives a clean proof that memories alter the event pool.

## Out of Scope

- large branching quest graphs
- tag negation / exclusion rules
- save persistence
- probabilistic narrative planners
