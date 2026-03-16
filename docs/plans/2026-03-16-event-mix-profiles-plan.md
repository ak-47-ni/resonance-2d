# Event Mix Profiles Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Add data-driven per-event mix profiles to content, validation, and demo runtime exposure without touching the audio backend.

**Architecture:** `EventData` gains a small `EventMixProfile` value object parsed from optional JSON. `DemoScene` stores the currently active event profile beside the current event id, and existing data/runtime tests verify that authored event profiles load and rotate correctly.

**Tech Stack:** `C++20`, `nlohmann_json`, `Python 3`, `CTest`

---

### Task 1: Lock content parsing with failing tests

**Files:**
- Modify: `tests/data/test_content_load.cpp`
- Modify: `assets/data/events/events.json`

**Step 1: Write the failing content test**

Assert that `distant_bell` and `passing_shadow` load distinct `mix_profile` values.

**Step 2: Run the focused test to verify it fails**

Run: `cmake --build build && ctest --test-dir build -R content_load --output-on-failure`
Expected: FAIL because `EventData` does not expose mix profiles yet.

### Task 2: Implement event mix profiles and validation

**Files:**
- Modify: `engine/event/EventData.h`
- Modify: `engine/event/EventData.cpp`
- Modify: `assets/data/events/events.json`
- Modify: `tools/validate_assets.py`

**Step 1: Add a small event mix profile struct**

Use neutral defaults when JSON omits the object.

**Step 2: Parse `mix_profile` from JSON**

Support `event_duck` and `ambient_boost` as optional authored fields.

**Step 3: Validate the new shape**

Reject malformed `mix_profile` objects and out-of-range values.

**Step 4: Run focused verification**

Run: `cmake --build build && ctest --test-dir build -R content_load --output-on-failure && python3 tools/validate_assets.py`
Expected: PASS.

### Task 3: Expose active event mix profiles in the runtime

**Files:**
- Modify: `game/demo/DemoScene.h`
- Modify: `game/demo/DemoScene.cpp`
- Modify: `tests/demo/test_demo_event_rotation.cpp`

**Step 1: Write the failing runtime assertion**

Verify meadow starts on `distant_bell` with its authored profile, then rotates to `passing_shadow` with a different profile.

**Step 2: Run the focused test to verify it fails**

Run: `ctest --test-dir build -R demo_event_rotation --output-on-failure`
Expected: FAIL because `DemoScene` does not expose the active event profile yet.

**Step 3: Implement minimal runtime exposure**

Store the chosen event profile whenever `current_event_id_` changes and add a const accessor.

**Step 4: Run focused verification**

Run: `cmake --build build && ctest --test-dir build -R 'content_load|demo_event_rotation' --output-on-failure`
Expected: PASS.

### Task 4: Update docs and run regression

**Files:**
- Modify: `docs/demo-walkthrough.md`
- Modify: `docs/verification-checklist.md`

**Step 1: Update docs**

Document that events now carry authored mix profiles, even though backend consumption is deferred.

**Step 2: Run full verification**

Run: `ctest --test-dir build --output-on-failure && python3 tools/validate_assets.py`
Expected: PASS.
