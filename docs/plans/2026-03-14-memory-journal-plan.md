# Memory Journal Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Record discovered story anchors as a lightweight journal and allow the player to toggle the journal view in the demo runtime.

**Architecture:** `DemoScene` owns discovered-memory entries and exposes journal state. `DebugOverlay` formats either the normal HUD or a journal view, while `main.cpp` maps `J` to the journal toggle.

**Tech Stack:** `C++20`, `SDL3`, `CMake`, `CTest`

---

### Task 1: Add journal state coverage

**Files:**
- Modify: `game/demo/DemoScene.h`
- Create: `tests/demo/test_memory_journal.cpp`
- Modify: `CMakeLists.txt`

**Step 1: Write the failing test**

Verify that:
- interacting with `meadow-swing` records one memory entry
- repeating the same interaction does not duplicate it
- the stored entry keeps the anchor id and story text

**Step 2: Run test to verify it fails**

Run: `cmake --build build && ctest --test-dir build -R memory_journal --output-on-failure`
Expected: FAIL because journal APIs do not exist.

**Step 3: Write minimal implementation**

Add a `MemoryJournalEntry` struct, a discovered-memory vector, and a `memory_journal_entries()` accessor.

**Step 4: Run test to verify it passes**

Run: `cmake --build build && ctest --test-dir build -R memory_journal --output-on-failure`
Expected: PASS.

### Task 2: Add journal overlay mode

**Files:**
- Modify: `engine/debug/DebugOverlay.h`
- Modify: `engine/debug/DebugOverlay.cpp`
- Modify: `game/demo/DemoScene.cpp`
- Modify: `game/demo/main.cpp`
- Create: `tests/demo/test_journal_overlay.cpp`
- Modify: `CMakeLists.txt`

**Step 1: Write the failing test**

Verify that after discovering a memory and toggling the journal, overlay lines include `Journal`, the entry id, and story text.

**Step 2: Run test to verify it fails**

Run: `cmake --build build && ctest --test-dir build -R journal_overlay --output-on-failure`
Expected: FAIL because journal overlay formatting and toggle do not exist.

**Step 3: Write minimal implementation**

Add a journal-overlay builder, journal toggle state in `DemoScene`, and bind `J` in the SDL loop.

**Step 4: Run test to verify it passes**

Run: `cmake --build build && ctest --test-dir build -R 'memory_journal|journal_overlay|demo_runtime|demo_visual_state' --output-on-failure`
Expected: PASS.

### Task 3: Full verification

**Files:**
- Modify: `docs/demo-walkthrough.md`

**Step 1: Update docs**

Document the `J` toggle and journal verification step.

**Step 2: Run full verification**

Run: `cmake -S . -B build -G Ninja && cmake --build build && ctest --test-dir build --output-on-failure && python3 tools/validate_assets.py && SDL_AUDIODRIVER=dummy SDL_VIDEODRIVER=dummy RESONANCE_DEMO_HIDDEN=1 RESONANCE_DEMO_AUTOCLOSE=1 ./build/resonance_demo`
Expected: PASS.
