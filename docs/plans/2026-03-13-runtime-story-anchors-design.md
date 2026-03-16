# Runtime Story Anchors Design

## Goal

Add lightweight, proximity-based narrative interaction points to the demo runtime so exploration can surface authored story fragments without disturbing the existing region, event, and audio state loop.

## Chosen Approach

### Recommended approach: data-driven story anchors in `DemoScene`

Use a new JSON content file to define small narrative anchors with world position, activation radius, prompt text, and story text. Load them into the existing demo content bundle, then let `DemoScene` resolve the nearest active anchor based on the player position. Interaction stays lightweight: the runtime only needs a prompt state and an `interact()` call.

This keeps the feature aligned with the current prototype:

- region and event systems stay intact
- audio remains driven by `music_state`
- rendering changes are limited to debug overlay text
- spatial interaction logic can evolve later into a richer gameplay layer

### Alternatives considered

#### Put story-anchor queries inside `World`

This would centralize spatial logic, but it would also force `World` to own more content types before the gameplay model is stable. For the current MVP, it is cleaner to keep `World` focused on region resolution and let `DemoScene` own the extra interaction layer.

#### Trigger story fragments only from random events

This would reuse existing event data, but it would blur two different feelings: random encounter versus deliberate exploration. The demo benefits from having both. Random events create atmosphere; anchors reward player intent.

## Architecture

### New content type

Add `StoryAnchorData` under `engine/world/` and load it from `assets/data/story/story_anchors.json`.

Each anchor should include:

- `id`
- `region_id`
- `x`
- `y`
- `activation_radius`
- `prompt_text`
- `story_text`

### Content loading

Extend `DemoContentBundle` so `load_demo_content()` returns story anchors together with regions, music states, and events.

### Runtime state

`DemoScene` owns:

- loaded `story_anchors`
- the current nearby anchor id
- the last triggered story anchor id
- the active story text

`update()` resolves the nearby anchor from player position. `interact()` consumes the nearby anchor and stores the resulting story text for overlay display.

### Overlay behavior

Extend `DebugOverlay` so it can show:

- current region
- current music state
- current event
- interaction prompt when the player is near an anchor
- active story text after interaction
- trace entries as before

## Data Flow

1. `load_demo_content("assets/data")` loads `story_anchors.json`
2. `DemoScene(bundle)` stores anchors locally
3. Movement updates player position
4. `DemoScene::update()` resolves region, event, music, and nearby anchor
5. `DemoScene::interact()` activates current anchor story text
6. `DebugOverlay::build_lines(...)` renders prompt/story text into the SDL debug panel

## Error Handling

- Missing or malformed `story_anchors.json` should fail loudly during content loading, just like the current JSON assets
- Validator should ensure:
  - every anchor references an existing region
  - `activation_radius` is positive
  - prompt and story text are non-empty strings
- If no anchor is nearby, `interact()` should be a safe no-op

## Testing Strategy

### Loader coverage

Add a data test for story-anchor content loading and bundle integration.

### Runtime coverage

Add a demo test that:

- positions the player near a known anchor
- verifies the prompt appears in overlay state
- calls `interact()`
- verifies the story text becomes active

### Overlay coverage

Add or update a debug-overlay test so prompt and story lines contribute to the overlay output and sizing.

## Out of Scope

- branching dialogue
- persistence / save data
- animated interaction markers
- music-reactive story pacing
- NLP or local LLM narration (`llama-cpp`, `sentencepiece`, `tensorrt-llm` are not directly applicable to this runtime-only step)

## Success Criteria

This feature is complete when:

- demo content includes authored story anchors
- moving near an anchor shows a prompt in the overlay
- pressing `E` in the SDL runtime triggers the anchor text
- tests cover loader, overlay, and runtime interaction behavior
- existing region/event/audio behavior remains green
