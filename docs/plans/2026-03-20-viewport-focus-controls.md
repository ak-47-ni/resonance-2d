# 2026-03-20 Viewport Focus Controls

## Goal
Add a small but high-value viewport control pass for Terminal A mainline: frame the current selection quickly and reset zoom without discarding the current framing origin.

## Scope
- `game/demo/DemoScene.*`
- `game/demo/main.cpp`
- `game/demo/WorldWorkspaceRenderModel.h`
- `tests/demo/test_demo_runtime.cpp`
- shared task board update only

## Controls
- `F` : reset viewport origin + zoom back to default framing
- `Shift + F` : focus/frame the current selected region or story anchor
- `0` / keypad `0` : reset zoom back to `1.0x` while preserving current viewport origin

## What Landed
- `DemoScene` now exposes `focus_editor_viewport_on_selection()` for region/anchor framing
- `DemoScene` now exposes `reset_editor_viewport_zoom()` for zoom-only reset
- selection focus computes center framing against real editor viewport content bounds, with bounded zoom and padding
- `main.cpp` routes `Shift+F` to selection focus and `0` to zoom-only reset
- edit overlay control copy now advertises `Shift+F frame` and `0 zoom`
- selected region/anchor viewport hints now surface `[Shift+F] frame // F5 save`

## Validation
- `cmake --build build --target demo_runtime_test`
- `./build/demo_runtime_test`
- `ctest --test-dir build -R 'demo_runtime|demo_editor_save|world_workspace_state|editor_document' --output-on-failure`
- `SDL_AUDIODRIVER=dummy SDL_VIDEODRIVER=dummy RESONANCE_DEMO_HIDDEN=1 RESONANCE_DEMO_AUTOCLOSE=1 ./build/resonance_demo`

## Notes
- No changes to `engine/audio/*`, persistence helpers, or `CMakeLists.txt`
- Dirty live asset payloads remain untouched
