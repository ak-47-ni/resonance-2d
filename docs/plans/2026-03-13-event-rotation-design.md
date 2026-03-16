# Event Rotation Design

## Goal

Reduce repetitive random-event behavior so exploration feels less mechanical. The runtime should not re-trigger the same highest-weight event every update in the same region.

## Chosen Approach

Use a lightweight event cooldown plus recent-event exclusion.

- `DemoScene` keeps a small amount of event runtime state: the active event, the last event id for the current region, and a simple elapsed counter.
- `EventDirector` remains the selection policy layer. It should:
  - refuse to produce a new event until a cooldown threshold is met
  - prefer region events not present in a recent-exclusion set
  - fall back to the best event if every candidate is excluded

This preserves deterministic tests while making exploration feel less repetitive.

## Why this approach

- Minimal API growth
- Keeps randomness policy in `engine/event/`
- Avoids event flicker in the overlay and audio state
- Leaves room for future weighted randomness or region-specific pacing

## Data Flow

1. `DemoScene::update()` tracks time since the last major event
2. `DemoScene` passes current region, cooldown time, and recent event ids into `EventDirector`
3. `EventDirector` either returns `nullopt` during cooldown or the best eligible event after cooldown
4. `DemoScene` keeps the current event active until a new one is selected or the player leaves the region

## Testing

- `tests/event/test_event_director.cpp` covers cooldown and exclusion behavior
- `tests/demo/test_demo_event_rotation.cpp` covers runtime persistence and rotation in the demo scene

## Out of Scope

- true stochastic randomness
- per-region probability curves
- save/load persistence of event history
