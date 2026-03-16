# Hidden Story Route Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Add a scripted hidden memory-chain route to `resonance_demo` so terminal output can verify story progression end-to-end.

**Architecture:** Keep the change inside the existing demo binary. Define a tiny shared route helper with fixed positions and labels, then let `main.cpp` execute it when `RESONANCE_DEMO_ROUTE=memory_chain` is present in hidden mode. Reuse an existing demo test file so no `CMakeLists.txt` change is required.

**Tech Stack:** C++20, existing `DemoScene`, SDL3 hidden demo path, CTest

---

### Task 1: Add the failing route test

**Files:**
- Create: `game/demo/DebugRoute.h`
- Modify: `tests/demo/test_demo_visual_state.cpp`

**Step 1: Write the failing test**
- Extend `tests/demo/test_demo_visual_state.cpp` to run the shared memory-chain route and assert the labeled summaries include `platform_convergence` and `terminal_refrain`.

**Step 2: Run test to verify it fails**
- Run: `cmake --build build --target demo_visual_state_test -j4 && ctest --test-dir build --output-on-failure -R demo_visual_state`
- Expected: FAIL because the route helper does not exist yet.

**Step 3: Write minimal implementation**
- Add `game/demo/DebugRoute.h` with fixed route steps and a small helper that drives `DemoScene` and collects labeled summaries.

**Step 4: Run test to verify it passes**
- Run the same command and expect PASS.

### Task 2: Wire hidden demo route output

**Files:**
- Modify: `game/demo/main.cpp`
- Modify: `docs/demo-walkthrough.md`
- Modify: `docs/verification-checklist.md`

**Step 1: Write the failing expectation in existing test surface**
- Extend `tests/demo/test_demo_visual_state.cpp` to assert the route labels are stable enough for hidden output reuse.

**Step 2: Run focused test to verify red state if needed**
- Run the same `demo_visual_state` command if additional assertions are added.

**Step 3: Write minimal implementation**
- In hidden mode, if `RESONANCE_DEMO_ROUTE=memory_chain`, print the scripted route summaries instead of only the final summary.

**Step 4: Run focused verification**
- Run: `ctest --test-dir build --output-on-failure -R 'demo_visual_state|demo_hidden_runtime'`
- Expected: PASS

### Task 3: Full verification

**Files:**
- Verify only

**Step 1: Run full verification**
- Run: `ctest --test-dir build --output-on-failure && /opt/homebrew/bin/python3 tools/validate_assets.py`
- Expected: all 25 tests pass and asset validation passes.
