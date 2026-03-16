# Event Rotation Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Make region events persist briefly and rotate away from immediate repeats during exploration.

**Architecture:** Extend `EventContext` with recent-event exclusions, let `EventDirector` enforce cooldown plus best-eligible selection, and let `DemoScene` preserve active event state between updates.

**Tech Stack:** `C++20`, `CMake`, `CTest`

---

### Task 1: Add selection-policy coverage

**Files:**
- Modify: `engine/event/EventContext.h`
- Modify: `tests/event/test_event_director.cpp`

**Step 1: Write the failing test**

Cover three behaviors:
- first pick in `meadow` returns `distant_bell`
- pick during cooldown returns no event
- pick after cooldown with `distant_bell` excluded returns `passing_shadow`

**Step 2: Run test to verify it fails**

Run: `cmake --build build && ctest --test-dir build -R event_director --output-on-failure`
Expected: FAIL because recent-event exclusions do not exist yet.

**Step 3: Write minimal implementation**

Extend `EventContext` and update `EventDirector` selection logic.

**Step 4: Run test to verify it passes**

Run: `cmake --build build && ctest --test-dir build -R event_director --output-on-failure`
Expected: PASS.

### Task 2: Add runtime persistence and rotation

**Files:**
- Modify: `game/demo/DemoScene.h`
- Modify: `game/demo/DemoScene.cpp`
- Create: `tests/demo/test_demo_event_rotation.cpp`
- Modify: `CMakeLists.txt`

**Step 1: Write the failing test**

Verify the demo scene:
- keeps the first event active on immediate re-update
- rotates to a different meadow event after enough updates in place

**Step 2: Run test to verify it fails**

Run: `cmake --build build && ctest --test-dir build -R demo_event_rotation --output-on-failure`
Expected: FAIL because runtime event persistence/rotation is not implemented.

**Step 3: Write minimal implementation**

Store active-event state and a simple update-based cooldown timer in `DemoScene`.

**Step 4: Run test to verify it passes**

Run: `cmake --build build && ctest --test-dir build -R 'demo_event_rotation|demo_runtime|demo_visual_state' --output-on-failure`
Expected: PASS.

### Task 3: Full verification

**Files:**
- Modify: `docs/demo-walkthrough.md`

**Step 1: Update docs**

Document event rotation expectations.

**Step 2: Run full verification**

Run: `cmake -S . -B build -G Ninja && cmake --build build && ctest --test-dir build --output-on-failure && python3 tools/validate_assets.py`
Expected: PASS.
