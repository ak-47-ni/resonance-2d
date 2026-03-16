# Story Anchor Markers Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Render lightweight visual markers for story anchors in the SDL demo and expose their visual state through `DemoScene`.

**Architecture:** `DemoScene` exposes a pure visual-state vector derived from story-anchor content and current interaction state. `game/demo/main.cpp` renders markers from that vector without reimplementing proximity logic.

**Tech Stack:** `C++20`, `SDL3`, `CMake`, `CTest`

---

### Task 1: Add visual-state coverage

**Files:**
- Modify: `game/demo/DemoScene.h`
- Create: `tests/demo/test_story_anchor_visuals.cpp`
- Modify: `CMakeLists.txt`

**Step 1: Write the failing test**

Verify that:
- the scene exposes all story anchors as visuals
- `meadow-swing` becomes `is_nearby` near `(96, 80)`
- after interaction it becomes `is_active`

**Step 2: Run test to verify it fails**

Run: `cmake --build build && ctest --test-dir build -R story_anchor_visuals --output-on-failure`
Expected: FAIL because the visual-state API does not exist.

**Step 3: Write minimal implementation**

Add a small `StoryAnchorVisual` data struct and a `story_anchor_visuals()` accessor.

**Step 4: Run test to verify it passes**

Run: `cmake --build build && ctest --test-dir build -R story_anchor_visuals --output-on-failure`
Expected: PASS.

### Task 2: Render markers in SDL demo

**Files:**
- Modify: `game/demo/main.cpp`
- Modify: `tests/demo/test_demo_visual_state.cpp`
- Modify: `docs/demo-walkthrough.md`

**Step 1: Update runtime surface**

Render small world-space markers for every anchor, highlighting nearby and active states.

**Step 2: Verify existing demo tests stay green**

Run: `cmake --build build && ctest --test-dir build -R 'story_anchor_visuals|demo_visual_state|story_anchor_interaction|demo_runtime' --output-on-failure`
Expected: PASS.

### Task 3: Full verification

**Files:**
- None beyond prior tasks

**Step 1: Run full verification**

Run: `cmake -S . -B build -G Ninja && cmake --build build && ctest --test-dir build --output-on-failure && python3 tools/validate_assets.py && SDL_AUDIODRIVER=dummy SDL_VIDEODRIVER=dummy RESONANCE_DEMO_HIDDEN=1 RESONANCE_DEMO_AUTOCLOSE=1 ./build/resonance_demo`
Expected: PASS.
