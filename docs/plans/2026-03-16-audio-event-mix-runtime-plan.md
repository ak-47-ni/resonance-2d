# Audio Event Mix Runtime Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Consume authored per-event mix scalars in `AudioDirector` so runtime audio gains differ by narrative event and can be verified in debug output.

**Architecture:** Keep event-domain data outside the backend. `DemoScene` passes the selected event's duck/boost scalars into `AudioDirector`, `AudioDirector` resolves final gains into the existing snapshot, and the backend continues to apply those gains without knowing event semantics. Debug output exposes the resolved gains for end-to-end verification.

**Tech Stack:** C++20, SDL3 runtime, existing `AudioDirector` snapshot path, CTest

---

### Task 1: Write failing tests

**Files:**
- Modify: `tests/audio/test_audio_state.cpp`
- Modify: `tests/demo/test_event_audio_emphasis.cpp`

**Step 1: Extend the audio snapshot test**
- Assert that the same event emphasis produces different `bgm_gain` / `ambient_gain_multiplier` once authored event mix scalars are applied.

**Step 2: Extend the runtime test**
- Assert that meadow's first event resolves to stronger final gains than the rotated meadow event.

**Step 3: Run tests to verify red state**
- Run: `cmake --build build --target audio_state_test event_audio_emphasis_test -j4 && ctest --test-dir build --output-on-failure -R 'audio_state|event_audio_emphasis'`
- Expected: FAIL because authored event mix is not yet consumed in runtime gains.

### Task 2: Implement minimal runtime path

**Files:**
- Modify: `engine/audio/AudioDirector.h`
- Modify: `engine/audio/AudioDirector.cpp`
- Modify: `game/demo/DemoScene.cpp`
- Modify: `engine/debug/DebugOverlay.h`
- Modify: `engine/debug/DebugOverlay.cpp`

**Step 1: Add authored event mix state to `AudioDirector`**
- Store `event_duck_scalar` and `ambient_boost_scalar` with sane defaults of `1.0`.

**Step 2: Feed selected event mix from `DemoScene`**
- When an event is selected, pass its profile scalars into `AudioDirector`.
- When leaving a region or clearing an event, reset the scalars to neutral.

**Step 3: Resolve final gains in `refresh_snapshot()`**
- Keep current story-focus and event-emphasis behavior, but scale the event contribution by authored mix values.

**Step 4: Expose resolved gains in debug text**
- Add `Bgm Gain` / `Ambient Gain` overlay lines for deterministic verification.

**Step 5: Run focused tests**
- Run: `ctest --test-dir build --output-on-failure -R 'audio_state|event_audio_emphasis|demo_runtime|overlay_panel'`
- Expected: PASS.

### Task 3: Refresh docs and verify all

**Files:**
- Modify: `docs/demo-walkthrough.md`
- Modify: `docs/verification-checklist.md`
- Modify: `docs/runtime-context.md`

**Step 1: Update docs**
- Note that authored event mix profiles now affect resolved runtime gains, not just telemetry.

**Step 2: Run full verification**
- Run: `ctest --test-dir build --output-on-failure && /opt/homebrew/bin/python3 tools/validate_assets.py`
- Expected: all tests pass and asset validation passes.
