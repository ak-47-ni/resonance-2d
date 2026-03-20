# Viewport Interaction Feedback Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Add hover, drag-state, and simple handle feedback to the `world_workspace` viewport so editing reads as interactive, not only stateful.

**Architecture:** Keep live interaction state in `DemoScene`, pass minimal hover/drag summary into `WorldWorkspaceState`, derive viewport copy from `WorldWorkspaceRenderModel`, and draw lightweight in-canvas feedback in `game/demo/main.cpp`. Avoid persistence/schema changes.

**Tech Stack:** C++20, SDL3, CMake, existing `DemoScene` editor loop, runtime test harness

---

### Task 1: Define failing tests for hover and drag feedback

**Files:**
- Modify: `tests/demo/test_demo_runtime.cpp`
- Test: `tests/demo/test_demo_runtime.cpp`

**Step 1: Write the failing test**
- Assert `DemoScene` can expose hover and drag state for editor mode.
- Assert render model focus/hint copy changes for hover region, hover anchor, and dragging anchor/region.

**Step 2: Run test to verify it fails**
Run: `cmake --build build --target demo_runtime_test`
Expected: compile/test failure on missing members or wrong values.

**Step 3: Write minimal implementation**
- Add state fields and render-model branching only.

**Step 4: Run test to verify it passes**
Run: `ctest --test-dir build -R '^demo_runtime$' --output-on-failure`
Expected: PASS

### Task 2: Render hover/handle/drag cues in SDL shell

**Files:**
- Modify: `game/demo/DemoScene.h`
- Modify: `game/demo/DemoScene.cpp`
- Modify: `game/demo/main.cpp`
- Modify: `game/demo/WorldWorkspaceRenderModel.h`
- Modify: `engine/editor/WorldWorkspaceState.h`
- Modify: `engine/editor/WorldWorkspaceState.cpp`

**Step 1: Keep tests red until behavior is wired**
- Do not add drawing-only code without state passing first.

**Step 2: Implement minimal behavior**
- Track hovered region/anchor and drag-active state.
- Draw faint hover outlines.
- Draw simple selected handles for region resize and anchor radius.
- Switch viewport focus chip to drag-aware copy.

**Step 3: Run focused verification**
Run: `ctest --test-dir build -R '^demo_runtime$' --output-on-failure`
Expected: PASS

### Task 3: Document and hand off

**Files:**
- Create: `docs/plans/2026-03-19-viewport-interaction-feedback.md`
- Modify: `/Users/ljs/resonance-2d/.git/local/task-board.md`

**Step 1: Run full focused suite**
Run: `ctest --test-dir build -R 'world_workspace_state|demo_runtime|demo_editor_save|editor_document' --output-on-failure`
Expected: PASS

**Step 2: Run hidden smoke**
Run: `SDL_AUDIODRIVER=dummy SDL_VIDEODRIVER=dummy RESONANCE_DEMO_HIDDEN=1 RESONANCE_DEMO_AUTOCLOSE=1 ./build/resonance_demo`
Expected: exits `0`

**Step 3: Record handoff**
- Write a short note describing what hover/drag/handle feedback now exists and what still remains.
