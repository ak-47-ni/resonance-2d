# Event Mix Profiles Design

## Goal

Add data-driven per-event mix profiles so narrative events can carry distinct audio personalities, even before those profiles are fully consumed by the backend layer.

## Context

The current runtime already supports two global mix shapers:

- story-focus proximity near story anchors
- event-emphasis ducking/fade when a new event appears

Right now every event shares the same emphasis behavior. That makes the runtime feel more reactive, but not yet more authored. The next step is to let individual events describe how strong their ducking and ambient lift should feel.

## Constraints

- Avoid conflicts with Terminal B, which currently owns `engine/audio/*` and `tests/audio/*`
- Keep this slice inside event data, validation, and runtime exposure
- Do not change the audio backend in this step
- Preserve current behavior for events that do not define a custom mix profile

## Chosen Approach

### Option A — Add optional event mix profiles to content and runtime state

Add an optional `mix_profile` object to each event with two normalized fields:

- `event_duck`: scales how strongly a narrative event should duck BGM
- `ambient_boost`: scales how strongly a narrative event should lift ambient layers

`EventData` parses and stores this profile, `validate_assets.py` checks it, and `DemoScene` exposes the currently selected event profile for future audio integration.

### Why this option

- keeps authored intent close to event content
- avoids touching Terminal B-owned backend files
- creates a clean bridge for later audio integration without reworking event JSON again
- is easy to verify in data and runtime tests

## Not Chosen

### Option B — Hardcode event-specific values in `DemoScene`

Rejected because it hides authored intent in C++ and makes future content changes cumbersome.

### Option C — Wait until backend work is free and do everything at once

Rejected because it would stall the mainline even though the content/runtime half can be delivered safely now.

## Data Shape

Example:

```json
{
  "id": "distant_bell",
  "region_id": "meadow",
  "requested_music_state": "mysterious",
  "weight": 5,
  "mix_profile": {
    "event_duck": 0.85,
    "ambient_boost": 0.65
  }
}
```

Rules:

- `mix_profile` is optional
- if omitted, runtime uses neutral defaults of `1.0` / `1.0`
- if present, both `event_duck` and `ambient_boost` must be numbers in `[0.0, 1.0]`

## Runtime Behavior

This slice does **not** change audio playback yet.

Instead it adds runtime visibility:

- `DemoScene` keeps the active event's resolved mix profile alongside `current_event_id_`
- tests can verify that event rotation also changes the selected profile
- a later integration step can feed this profile into `AudioDirector` once Terminal B's backend branch is clear

## Testing

- extend `tests/data/test_content_load.cpp` to verify event mix profiles load from JSON
- extend `tests/demo/test_demo_event_rotation.cpp` to verify the active event profile changes when meadow rotates from `distant_bell` to `passing_shadow`
- run `python3 tools/validate_assets.py` to verify schema validation

## Out of Scope

- consuming event mix profiles in `engine/audio/*`
- backend crossfade changes
- config menus or user-facing audio sliders
