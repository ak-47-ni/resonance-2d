# Audio Assets Phase 1 Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Wire `music_state` into real `WAV` BGM asset playback with tone fallback, while keeping runtime changes small and localized to the audio subsystem.

**Architecture:** `AudioDirector` resolves logical music states into a richer playback snapshot, and `AudioBackendSDL` consumes that snapshot to choose between buffered `WAV` playback and synthesized tone fallback. Validation and tests guard the new asset references without touching world or event systems.

**Tech Stack:** C++20, SDL audio backend, CMake/Ninja, Python 3 asset validation, JSON content assets.

---

### Task 1: Lock the new behavior with failing tests

**Files:**
- Modify: `tests/audio/test_audio_state.cpp`
- Create: `tests/audio/test_audio_asset_validator.py`
- Modify: `CMakeLists.txt`

**Step 1: Write the failing C++ test**

Extend `tests/audio/test_audio_state.cpp` to verify that a registered music state resolves to a `.wav` BGM path and that a missing asset path still leaves a fallback music state available.

**Step 2: Run test to verify it fails**

Run: `ctest --test-dir build --output-on-failure -R 'audio_state'`
Expected: FAIL because `AudioStateSnapshot` and `AudioDirector` do not yet expose resolved asset information.

**Step 3: Write the failing validator test**

Add `tests/audio/test_audio_asset_validator.py` with subprocess checks for:

- a missing `bgm_track` file
- a non-`.wav` `bgm_track`

**Step 4: Run test to verify it fails**

Run: `python3 tests/audio/test_audio_asset_validator.py`
Expected: FAIL because `tools/validate_assets.py` does not yet validate file existence or extension.

**Step 5: Commit**

Skip in this session unless explicitly requested.

### Task 2: Resolve music states into playback snapshots

**Files:**
- Modify: `engine/audio/AudioBackend.h`
- Modify: `engine/audio/AudioDirector.h`
- Modify: `engine/audio/AudioDirector.cpp`
- Modify: `game/demo/DemoScene.cpp`
- Modify: `tests/audio/test_audio_state.cpp`

**Step 1: Add snapshot fields**

Add `resolved_bgm_track` and `fallback_music_state` to `AudioStateSnapshot`.

**Step 2: Register music state data**

Add an `AudioDirector` API that ingests `MusicStateData` records and stores them by `id`.

**Step 3: Resolve snapshots in one place**

Update `refresh_snapshot()` to populate logical state, ambient layers, resolved track path, and fallback state from the registered content map.

**Step 4: Wire demo content**

Update `DemoScene(bundle)` so `bundle.music_states` is registered before runtime updates begin.

**Step 5: Run targeted tests**

Run: `ctest --test-dir build --output-on-failure -R 'audio_state|demo_runtime|demo_visual_state'`
Expected: PASS.

**Step 6: Commit**

Skip in this session unless explicitly requested.

### Task 3: Add `WAV` asset playback with tone fallback

**Files:**
- Modify: `engine/audio/AudioBackendSDL.cpp`
- Modify: `assets/data/music/music_states.json`
- Create: `assets/audio/bgm/explore.wav`
- Create: `assets/audio/bgm/mysterious.wav`
- Create: `assets/audio/bgm/calm.wav`

**Step 1: Add buffered asset state**

Teach `AudioBackendSDL` to cache the currently loaded asset path, decoded PCM samples, playback cursor, and fallback mode.

**Step 2: Load `WAV` assets lazily**

On `apply()`, if `resolved_bgm_track` changes, try to decode the `WAV` into the backend output format. If decoding fails, keep the stream alive and mark fallback mode.

**Step 3: Feed either PCM or synthesized tones**

In `feed_audio_locked()`, output buffered PCM samples when available; otherwise reuse the existing tone synthesis path.

**Step 4: Add demo `WAV` assets and update JSON**

Generate three short loopable `WAV` files and switch `music_states.json` references from `.ogg` to `.wav`.

**Step 5: Run targeted tests**

Run: `ctest --test-dir build --output-on-failure -R 'audio_state|audio_tone_profile|demo_hidden_runtime'`
Expected: PASS.

**Step 6: Commit**

Skip in this session unless explicitly requested.

### Task 4: Enforce asset validation and final regression

**Files:**
- Modify: `tools/validate_assets.py`
- Create: `tests/audio/test_audio_asset_validator.py`

**Step 1: Validate existence and extension**

Require every `bgm_track` to exist under `assets/` and end in `.wav`.

**Step 2: Run validator tests**

Run: `python3 tests/audio/test_audio_asset_validator.py`
Expected: PASS.

**Step 3: Run full project verification**

Run:

```bash
cmake -S . -B build -G Ninja
cmake --build build
ctest --test-dir build --output-on-failure
python3 tools/validate_assets.py
SDL_AUDIODRIVER=dummy SDL_VIDEODRIVER=dummy RESONANCE_DEMO_HIDDEN=1 RESONANCE_DEMO_AUTOCLOSE=1 ./build/resonance_demo
```

Expected: all commands succeed.

**Step 4: Commit**

Skip in this session unless explicitly requested.
