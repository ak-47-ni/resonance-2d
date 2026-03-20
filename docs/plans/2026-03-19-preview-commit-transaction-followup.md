# 2026-03-19 Preview Commit Transaction Follow-up

## Goal
Close Terminal A's minimal preview->commit editor transaction slice without touching inherited dirty asset payloads.

## Root Cause
Focused runtime/editor tests were assuming fixed `meadow` / `meadow-swing` coordinates and bounds, but the current worktree carries user-modified `assets/data` values. On top of that, some tests were reading `story_anchor_visuals()` through a temporary vector, which created a dangling pointer risk.

## What Changed
- Switched runtime/save/editor tests from hardcoded world coordinates to content-derived pick points and expected overlay strings.
- Snapshotted region/anchor baselines before mutating the scene so save assertions compare against stable initial values.
- Kept the preview->commit/cancel drag implementation unchanged; verified move, resize, and radius previews stay non-destructive until commit.

## Validation
- `ctest --test-dir build -R 'demo_runtime|demo_editor_save|world_workspace_state|editor_document' --output-on-failure`
- `SDL_AUDIODRIVER=dummy SDL_VIDEODRIVER=dummy RESONANCE_DEMO_HIDDEN=1 RESONANCE_DEMO_AUTOCLOSE=1 ./build/resonance_demo`

## Notes For Next Slice
- Current mainline preview->commit behavior is validated through dynamic tests and hidden demo smoke.
- If we add undo/redo later, this slice can become the baseline transaction semantics instead of reusing old immediate-mutate assumptions.
