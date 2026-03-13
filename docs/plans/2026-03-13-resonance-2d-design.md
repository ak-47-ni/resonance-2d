# Resonance 2D Design

**Date:** 2026-03-13

**Project Name:** `resonance-2d`

**Positioning:** A lightweight 2D engine for narrative exploration games with open-world flavor, random events, and music-driven immersion.

## Goals

- Start as a learning-oriented engine with a small, understandable codebase.
- Prioritize atmosphere, music immersion, and narrative context over advanced rendering.
- Support small open-world scenes with region-driven ambience and random story events.
- Keep the runtime in `C++` and use `Python` for offline tools.

## Non-Goals for Phase 1

- Full editor or visual scripting.
- Advanced physics, ECS-first architecture, or large-world streaming.
- Complex DSP, synthesis, or audio workstation features.
- Production-grade content pipeline or packaging.

## Core Idea

The engine should make it easy to build games where music and environmental sound react to place, story state, and random narrative events. The distinctive part of the engine is not low-level audio processing. It is the coordination between world state, event selection, and audio state transitions.

## Technical Stack

- `C++20`
- `SDL3`
- `SDL3_image`
- `SDL3_mixer`
- `CMake`
- `Python 3` for tooling
- `JSON` data files for phase 1 content

## Engine Shape

The project will be structured as:

- An engine runtime library
- A small demo game used to validate the engine
- Lightweight debug tools embedded in the running game
- Offline Python tools for validation and content preprocessing

## High-Level Modules

### Core

Responsibilities:

- Main loop
- Time step and frame timing
- Logging
- Basic configuration

### Render2D

Responsibilities:

- Window and renderer setup
- Camera
- Sprite drawing
- Layer ordering
- Debug text and simple overlays

### World

Responsibilities:

- Map and region definitions
- Trigger volumes
- Region transitions
- World tags and story flags

### Event Director

Responsibilities:

- Evaluate eligible random/narrative events
- Respect conditions, weights, and cooldowns
- Trigger event outcomes
- Request audio and gameplay state changes

### Audio Director

Responsibilities:

- Resolve current music state
- Manage ambient loop layers
- Play one-shot cues
- Handle fade and crossfade behavior

### Gameplay

Responsibilities:

- Player movement and interaction
- Demo-specific narrative state
- Event acknowledgements and simple UI prompts

### Debug Tools

Responsibilities:

- Current region display
- Current music state display
- Recent event log
- Reason tracing for event/audio decisions

### Python Tools

Responsibilities:

- Validate content files
- Build or normalize event tables
- Check references between regions, audio states, and assets

## Runtime Data Flow

The phase 1 update loop is:

1. Input update
2. World update
3. Event Director update
4. Audio Director update
5. Render
6. Debug overlay

`World` provides region context and world tags.

`Event Director` decides whether a contextual event should happen.

`Audio Director` turns world context and event cues into the actual heard result.

This creates the intended feel:

- Regions define baseline mood.
- Events create dramatic shifts.
- Music and ambience deepen narrative immersion.

## Content Model

Phase 1 content will be data-driven with JSON files:

- `assets/data/regions/regions.json`
- `assets/data/events/events.json`
- `assets/data/music/music_states.json`

These files describe:

- Regions and their default ambience/music
- Event conditions, weights, cooldowns, and outputs
- Music states and transition rules

## Project Layout

```text
resonance-2d/
├── CMakeLists.txt
├── engine/
│   ├── core/
│   ├── render/
│   ├── audio/
│   ├── world/
│   ├── event/
│   └── debug/
├── game/
│   └── demo/
├── assets/
│   ├── audio/
│   │   ├── bgm/
│   │   ├── ambient/
│   │   └── sfx/
│   ├── maps/
│   ├── sprites/
│   └── data/
│       ├── events/
│       ├── music/
│       └── regions/
├── tools/
└── docs/
    └── plans/
```

## MVP

The first playable milestone should prove the engine concept with:

- One small map
- Three to five regions
- Five to ten narrative/random events
- Three music states
- Ambient sound per region
- Debug overlay showing why the current state is active

## Phase 1 Milestones

### M0: Project bootstrap

- CMake project
- SDL integration
- Folder structure
- Minimal app startup

### M1: Runtime basics

- Window
- Input
- Timing
- Camera
- Placeholder player movement

### M2: World basics

- Region definitions
- Trigger checks
- Region change notification

### M3: Audio Director v1

- Region-based music state
- Ambient loop handling
- One-shot cue playback
- Fade and crossfade handling

### M4: Event Director v1

- Eligibility checks
- Weight-based selection
- Cooldowns
- Event-to-audio requests

### M5: Debug tools

- Overlay panels
- Recent event log
- Current region/music state
- Decision trace information

### M6: Demo scene

- Small exploration space
- Distinct mood zones
- Noticeable random narrative moments

## Error Handling Strategy

- Missing resources fall back to placeholders or silence and emit clear logs.
- Invalid content files fail early with file path and field details.
- Audio failures must not crash the main loop.
- Invalid event references must be caught by offline validation.
- Debug output should make failures visible instead of silent.

## Testing Strategy

- Unit tests for event selection rules, cooldowns, and audio state resolution.
- Loader tests for content parsing and schema validation.
- Integration smoke test for startup and demo content loading.
- Manual validation for feel: transitions, ambience, and event cadence.

## Recommended First Implementation Scope

The first implementation pass should optimize for clarity, not scale:

- Small map
- Simple player controller
- Rectangular regions and triggers
- JSON-driven content
- Debug overlay always available in dev mode

## Future Extensions

After the MVP proves the concept, the engine can grow toward:

- Save/load of world narrative state
- More expressive trigger conditions
- NPC event hooks
- Better animation support
- Lightweight editor panels
- Optional scripting integration

## Notes

- `C++` is the runtime foundation.
- `Python` stays in the tool layer at first.
- The distinctive feature is the coupling of narrative context and music immersion.
- The project name can still be renamed later if needed.
