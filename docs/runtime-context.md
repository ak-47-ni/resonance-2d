# Runtime Context Snapshot

_Last updated: 2026-03-16, Terminal A (`feature/audio-proximity-immersion`)_

## Coordination

- Repo root: `/Users/ljs/resonance-2d`
- Terminal A worktree: `/Users/ljs/resonance-2d/.worktrees/audio-proximity-immersion`
- Terminal B worktree: `/Users/ljs/resonance-2d/.worktrees/terminal-b-next-task`
- Shared board: `/Users/ljs/resonance-2d/.git/local/task-board.md`
- Terminal B has released backend audio ownership; Terminal A can now continue `engine/audio/*` work on this branch

## Terminal A Scope

Terminal A should prefer runtime/debug/story-safe surfaces:

- `game/demo/*`
- `engine/debug/*`
- `engine/event/*`
- `tests/demo/*`
- `tests/debug/*`
- docs for walkthrough, verification, plans, and runtime status

Avoid new `engine/audio/*` edits until the board explicitly hands them back.

## Verified Runtime Features

- story-anchor proximity markers and interaction prompts
- story-focus + event-emphasis telemetry in overlay and summary
- authored event mix-profile telemetry exposure
- authored event mix profiles now affect resolved runtime BGM / ambient gains
- memory journal recording without duplicates
- journal entries now include source region context in journal view
- memory-chain progression and station upgrade hints
- visible-event pool and next-unlock telemetry
- journal view mirroring chain/unlock hints
- overlay/summary exposure of nearby and active anchor ids
- hidden scripted memory-chain route via `RESONANCE_DEMO_ROUTE=memory_chain`

## Key Commands

### Full verification

- `ctest --test-dir build --output-on-failure`
- `/opt/homebrew/bin/python3 tools/validate_assets.py`

### Hidden demo smoke

- `SDL_AUDIODRIVER=dummy SDL_VIDEODRIVER=dummy RESONANCE_DEMO_HIDDEN=1 RESONANCE_DEMO_AUTOCLOSE=1 build/resonance_demo`

### Hidden memory-chain route

- `SDL_AUDIODRIVER=dummy SDL_VIDEODRIVER=dummy RESONANCE_DEMO_HIDDEN=1 RESONANCE_DEMO_ROUTE=memory_chain build/resonance_demo`

Expected route output includes:

- `RouteStep=meadow-start`
- `RouteStep=station-stage-two`
- `RouteStep=station-stage-three`
- final event `terminal_refrain`

## Current Important Files

- route helper: `game/demo/DebugRoute.h`
- hidden route hook: `game/demo/main.cpp`
- overlay telemetry: `engine/debug/DebugOverlay.cpp`
- runtime summary: `game/demo/DemoScene.cpp`
- long-session coordination: `.git/local/task-board.md`

## Suggested Next Steps

1. journal context polish (`Region:` lines, maybe later friendlier names)
2. branch handoff prep once Terminal B audio backend work is ready
3. after merge, reopen from latest `main` on a fresh branch/worktree
