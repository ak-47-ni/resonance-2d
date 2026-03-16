# Audio Controls Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Add backend-owned global audio controls for master/BGM/ambient gain plus BGM crossfade, with focused automated coverage.

**Architecture:** Extend `AudioStateSnapshot` with four global control values, keep `AudioDirector` responsible for assembling state snapshots, and implement the new mixing behavior inside `AudioBackendSDL`. Use tests to lock snapshot defaults first, then add a backend-focused regression path that exercises control clamping and crossfade state transitions without depending on a real audio device.

**Tech Stack:** C++20, SDL3 audio stream backend, existing WAV asset loader, CMake/CTest, current lightweight test executables.

---

### Task 1: Lock the snapshot contract with failing tests

**Files:**
- Modify: `tests/audio/test_audio_state.cpp`
- Modify: `engine/audio/AudioBackend.h`

**Step 1: Write the failing assertions**

Extend `tests/audio/test_audio_state.cpp` to assert that snapshots expose:

- `master_volume == 1.0F`
- `bgm_volume == 1.0F`
- `ambient_volume == 1.0F`
- `crossfade_seconds == 1.0F`

for the default region-driven and story-override-driven snapshots.

**Step 2: Run the focused test to verify it fails**

Run: `ctest --test-dir build --output-on-failure -R '^audio_state$'`
Expected: FAIL because `AudioStateSnapshot` does not define the new control fields yet.

**Step 3: Add the minimal snapshot fields**

Modify `engine/audio/AudioBackend.h` so `AudioStateSnapshot` defines the four control fields with safe defaults.

**Step 4: Run the focused test to verify it passes**

Run: `ctest --test-dir build --output-on-failure -R '^audio_state$'`
Expected: PASS.

### Task 2: Add a backend-focused regression test harness

**Files:**
- Create: `tests/audio/test_audio_backend_controls.cpp`
- Modify: `CMakeLists.txt`
- Modify: `engine/audio/AudioBackendSDL.cpp`

**Step 1: Write the failing backend test**

Create `tests/audio/test_audio_backend_controls.cpp` with a focused executable that verifies:

- control values are clamped into valid ranges
- identical BGM tracks do not start a crossfade
- `crossfade_seconds == 0.0F` switches immediately
- changing BGM tracks starts a crossfade and the transition completes after enough simulated time

**Step 2: Add a minimal backend test seam**

Expose a narrow, test-only helper path from `AudioBackendSDL.cpp` so the test can advance the backend mix state deterministically without a real SDL device.

**Step 3: Register the new executable in CMake**

Add `audio_backend_controls_test` and wire it into `ctest`.

**Step 4: Run the new test to verify it fails for the right reason**

Run: `ctest --test-dir build --output-on-failure -R '^audio_backend_controls$'`
Expected: FAIL because the backend has not implemented control clamping and crossfade state yet.

### Task 3: Implement global gain handling in the backend

**Files:**
- Modify: `engine/audio/AudioBackendSDL.cpp`
- Test: `tests/audio/test_audio_backend_controls.cpp`

**Step 1: Add normalized control helpers**

Implement small helpers that clamp snapshot control values and compute effective gains for:

- master output
- primary BGM/tone path
- ambient layers

**Step 2: Apply control values to existing mixing paths**

Make sure:

- WAV BGM gain uses base BGM gain × `bgm_volume` × `master_volume`
- tone fallback gain uses `AudioToneProfile` output × `bgm_volume` × `master_volume`
- ambient layers use base ambient gain × `ambient_volume` × `master_volume`

**Step 3: Run the backend test**

Run: `ctest --test-dir build --output-on-failure -R '^audio_backend_controls$'`
Expected: still FAIL, now only on missing crossfade transition behavior.

### Task 4: Implement BGM crossfade state transitions

**Files:**
- Modify: `engine/audio/AudioBackendSDL.cpp`
- Test: `tests/audio/test_audio_backend_controls.cpp`

**Step 1: Add previous/current primary track state**

Track both the current primary source and the fading-out previous source, including assets, cursors, and transition timing.

**Step 2: Start crossfade only on real track changes**

When `resolved_bgm_track` changes to a different value:

- if `crossfade_seconds > 0.0F`, preserve the old primary source and start a transition
- otherwise replace immediately

**Step 3: Blend old and new primary sources during feed**

Advance the transition based on rendered sample time and crossfade linearly from old source to new source.

**Step 4: Complete and clear transition state**

Once elapsed time reaches total crossfade duration, drop the previous source and keep only the new primary source.

**Step 5: Run the backend test to verify it passes**

Run: `ctest --test-dir build --output-on-failure -R '^audio_backend_controls$'`
Expected: PASS.

### Task 5: Run focused and full verification

**Files:**
- Modify: `docs/plans/2026-03-16-audio-controls-design.md`
- Modify: `docs/plans/2026-03-16-audio-controls-plan.md`

**Step 1: Reconfigure and rebuild**

Run:

```bash
cmake -S . -B build -G Ninja
cmake --build build
```

Expected: configure and build succeed.

**Step 2: Run focused verification**

Run: `ctest --test-dir build --output-on-failure -R 'audio_state|audio_backend_controls|wav_audio_asset'`
Expected: PASS.

**Step 3: Run full regression**

Run:

```bash
ctest --test-dir build --output-on-failure
python3 tools/validate_assets.py
SDL_AUDIODRIVER=dummy SDL_VIDEODRIVER=dummy RESONANCE_DEMO_HIDDEN=1 RESONANCE_DEMO_AUTOCLOSE=1 ./build/resonance_demo
```

Expected: all commands succeed.

**Step 4: Prepare commit**

Run:

```bash
git -C /Users/ljs/resonance-2d/.worktrees/terminal-b-next-task status --short
```

Expected: only the intended audio control files and plan docs are modified.
