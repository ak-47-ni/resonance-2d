# Viewport Selection Feedback Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Add clearer selection/focus feedback inside the `world_workspace` viewport so the embedded editor feels more like a tool surface and less like a plain debug canvas.

**Architecture:** Extend the state-first `WorldWorkspaceRenderModel` with minimal viewport feedback tokens, add testable layout rects for in-viewport chrome, and let `game/demo/main.cpp` render those cues using existing scene/runtime state. Keep the work isolated to Terminal A mainline files and avoid editor-side schema changes.

**Tech Stack:** C++20, SDL3, CMake, existing `DemoScene`/workspace layout/render-model test harness

---

### Task 1: Define viewport feedback expectations

**Files:**
- Modify: `tests/demo/test_demo_runtime.cpp`
- Test: `tests/demo/test_demo_runtime.cpp`

**Step 1: Write the failing test**
- Assert new layout rects exist for a viewport focus chip and hint band.
- Assert render model exposes focus label/detail plus hint lines for world and anchor workspace states.

**Step 2: Run test to verify it fails**
Run: `ctest --test-dir build -R '^demo_runtime$' --output-on-failure`
Expected: FAIL because new layout/model members do not exist yet.

**Step 3: Write minimal implementation**
- Add the new layout rects and render-model fields only.

**Step 4: Run test to verify it passes**
Run: `ctest --test-dir build -R '^demo_runtime$' --output-on-failure`
Expected: PASS

### Task 2: Render viewport feedback chrome

**Files:**
- Modify: `game/demo/main.cpp`
- Modify: `game/demo/WorldWorkspaceLayout.h`
- Modify: `game/demo/WorldWorkspaceRenderModel.h`
- Test: `tests/demo/test_demo_runtime.cpp`

**Step 1: Write the failing test**
- Keep Task 1 assertions red until render-model values are wired.

**Step 2: Run test to verify it fails**
Run: `cmake --build build --target demo_runtime_test`
Expected: FAIL or test red on missing values.

**Step 3: Write minimal implementation**
- Render a focus chip and hint band inside the viewport.
- Use workspace accent tones and existing selection data.
- Add stronger selected-region and selected-anchor labels without changing interaction logic.

**Step 4: Run test to verify it passes**
Run: `ctest --test-dir build -R '^demo_runtime$' --output-on-failure`
Expected: PASS

### Task 3: Verify and document the slice

**Files:**
- Modify: `docs/plans/2026-03-19-viewport-selection-feedback-plan.md`
- Create: `docs/plans/2026-03-19-viewport-selection-feedback.md`
- Modify: `/Users/ljs/resonance-2d/.git/local/task-board.md`

**Step 1: Run focused verification**
Run: `ctest --test-dir build -R 'world_workspace_state|demo_runtime|demo_editor_save|editor_document' --output-on-failure`
Expected: PASS

**Step 2: Run hidden smoke**
Run: `SDL_AUDIODRIVER=dummy SDL_VIDEODRIVER=dummy RESONANCE_DEMO_HIDDEN=1 RESONANCE_DEMO_AUTOCLOSE=1 ./build/resonance_demo`
Expected: summary line prints and process exits `0`

**Step 3: Write short stage note**
- Capture scope, validation, and remaining gaps.

**Step 4: Update the shared board**
- Replace the Terminal A claim with a done note for this viewport feedback slice.
