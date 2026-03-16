# Journal Context Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Add region context to each journal entry line and maintain a runtime-context snapshot doc for long-session continuity.

**Architecture:** Keep the change display-only. `MemoryJournalEntry` already stores `region_id`, so tests should first assert it, then `DebugOverlay::build_journal_lines()` should render it. Add one stable markdown snapshot file describing the current runtime/debug state and verification flow.

**Tech Stack:** C++20, existing `DemoScene`/`DebugOverlay`, Markdown docs, CTest

---

### Task 1: Write failing journal-context tests

**Files:**
- Modify: `tests/demo/test_memory_journal.cpp`
- Modify: `tests/demo/test_journal_overlay.cpp`

**Step 1: Write the failing tests**
- Assert `memory_journal_entries().front().region_id == "meadow"`.
- Assert journal overlay contains `Region: meadow`.

**Step 2: Run tests to verify they fail**
- Run: `cmake --build build --target memory_journal_test journal_overlay_test -j4 && ctest --test-dir build --output-on-failure -R 'memory_journal|journal_overlay'`
- Expected: FAIL because journal overlay does not render region lines yet.

**Step 3: Write minimal implementation**
- Update `DebugOverlay::build_journal_lines()` to render one `Region:` line per entry.

**Step 4: Run tests to verify they pass**
- Run the same command and expect PASS.

### Task 2: Add runtime context snapshot documentation

**Files:**
- Create: `docs/runtime-context.md`
- Modify: `docs/demo-walkthrough.md`
- Modify: `docs/verification-checklist.md`

**Step 1: Write the runtime snapshot doc**
- Capture current branch/worktree, ownership boundaries, verified features, hidden route command, and next likely tasks.

**Step 2: Update user-facing docs**
- Mention that journal entries now include region context.

### Task 3: Full verification

**Files:**
- Verify only

**Step 1: Run full verification**
- Run: `ctest --test-dir build --output-on-failure && /opt/homebrew/bin/python3 tools/validate_assets.py`
- Expected: all 25 tests pass and asset validation passes.
