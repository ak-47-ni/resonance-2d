# Memory-Driven Events Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Allow discovered memories to unlock new region events through tag-based gating.

**Architecture:** `EventData` declares optional required tags, `EventDirector` filters candidates by those tags, and `DemoScene` derives runtime tags from discovered journal entries before requesting an event.

**Tech Stack:** `C++20`, `CMake`, `CTest`, `nlohmann_json`, `Python 3`

---

### Task 1: Add gated-event coverage

**Files:**
- Modify: `tests/event/test_event_director.cpp`
- Create: `tests/demo/test_memory_unlock_event.cpp`
- Modify: `CMakeLists.txt`

**Step 1: Write the failing tests**

Cover two behaviors:
- a gated meadow event stays unavailable without the `memory:meadow-swing` tag
- after discovering `meadow-swing`, the demo scene rotates into the unlocked event

**Step 2: Run tests to verify they fail**

Run: `cmake --build build && ctest --test-dir build -R 'event_director|memory_unlock_event' --output-on-failure`
Expected: FAIL because gated event data and tag propagation do not exist yet.

**Step 3: Write minimal implementation**

Add `required_world_tags` to `EventData`, parse it from JSON, propagate memory tags from `DemoScene`, and filter in `EventDirector`.

**Step 4: Run tests to verify they pass**

Run: `cmake --build build && ctest --test-dir build -R 'event_director|memory_unlock_event' --output-on-failure`
Expected: PASS.

### Task 2: Validate and document gated content

**Files:**
- Modify: `assets/data/events/events.json`
- Modify: `tools/validate_assets.py`
- Modify: `docs/demo-walkthrough.md`

**Step 1: Add gated event content**

Introduce at least one memory-gated event using `required_world_tags`.

**Step 2: Update validator**

Ensure `required_world_tags`, when present, is a string array of non-empty values.

**Step 3: Run full verification**

Run: `cmake -S . -B build -G Ninja && cmake --build build && ctest --test-dir build --output-on-failure && python3 tools/validate_assets.py && SDL_AUDIODRIVER=dummy SDL_VIDEODRIVER=dummy RESONANCE_DEMO_HIDDEN=1 RESONANCE_DEMO_AUTOCLOSE=1 ./build/resonance_demo`
Expected: PASS.
