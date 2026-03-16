# Event Ducking and Fade Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Add a small event-triggered emphasis layer that briefly ducks the music mix and decays through a short fade after a newly selected event.

**Architecture:** `DemoScene` already knows when an event changes, so it will trigger a short-lived event emphasis value. `AudioDirector` will merge that emphasis with the existing story-focus shaping into snapshot gain fields, and the SDL backend will smoothly interpolate toward the latest gain targets instead of stepping instantly. This keeps the logic deterministic and testable while staying inside the current runtime/audio split.

**Tech Stack:** `C++20`, `SDL3`, `CMake`, `CTest`

---

### Task 1: Lock event-emphasis behavior with failing tests

**Files:**
- Modify: `tests/audio/test_audio_state.cpp`
- Create: `tests/demo/test_event_audio_emphasis.cpp`
- Modify: `CMakeLists.txt`

**Step 1: Write the failing audio snapshot test**

Assert that applying event emphasis changes `bgm_gain` and `ambient_gain_multiplier`, then clearing it restores neutral values.

**Step 2: Run the focused tests to verify they fail**

Run: `cmake --build build && ctest --test-dir build -R 'audio_state|event_audio_emphasis' --output-on-failure`
Expected: FAIL because event emphasis fields/methods do not exist yet.

**Step 3: Write minimal runtime test**

Assert that after the first event appears, event emphasis is positive; after several updates it decays back toward zero.

**Step 4: Run the focused tests to verify they fail**

Run: `ctest --test-dir build -R 'audio_state|event_audio_emphasis' --output-on-failure`
Expected: FAIL because `DemoScene` does not expose event emphasis yet.

### Task 2: Implement event emphasis and decay

**Files:**
- Modify: `engine/audio/AudioBackend.h`
- Modify: `engine/audio/AudioDirector.h`
- Modify: `engine/audio/AudioDirector.cpp`
- Modify: `game/demo/DemoScene.h`
- Modify: `game/demo/DemoScene.cpp`

**Step 1: Add event-emphasis state to the audio snapshot**

Keep the existing story-focus fields and add a small event-emphasis scalar that contributes additional ducking/boost.

**Step 2: Add event-emphasis controls to `AudioDirector`**

Expose a setter and merge event emphasis into the resolved snapshot gains.

**Step 3: Trigger and decay emphasis in `DemoScene`**

When a newly selected event replaces the current one, raise emphasis to a fixed value. On each update, decay it back toward zero.

**Step 4: Run focused tests**

Run: `cmake --build build && ctest --test-dir build -R 'audio_state|event_audio_emphasis|demo_event_rotation' --output-on-failure`
Expected: PASS.

### Task 3: Add short backend fade smoothing

**Files:**
- Modify: `engine/audio/AudioBackendSDL.cpp`
- Modify: `docs/demo-walkthrough.md`
- Modify: `docs/verification-checklist.md`

**Step 1: Smooth gain changes in the SDL backend**

Interpolate the live BGM and ambient gains toward snapshot targets over a short window instead of switching instantly.

**Step 2: Update docs**

Document the new event-triggered emphasis behavior and the remaining limitations.

**Step 3: Run full verification**

Run: `ctest --test-dir build --output-on-failure && python3 tools/validate_assets.py && SDL_AUDIODRIVER=dummy SDL_VIDEODRIVER=dummy RESONANCE_DEMO_HIDDEN=1 RESONANCE_DEMO_AUTOCLOSE=1 ./build/resonance_demo`
Expected: PASS.
