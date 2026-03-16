# Audio Ambient Layers Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Add real ambient layer playback on top of phase-1 `WAV` BGM playback, with missing ambient assets ignored at runtime and enforced by validation.

**Architecture:** `AudioDirector` resolves ambient layer ids into concrete `WAV` asset paths, `AudioBackendSDL` mixes up to two looping ambient assets into the existing output stream, and validator/tests lock the contract.

**Tech Stack:** C++20, SDL3 audio stream, CMake/Ninja, Python 3 validator tests, JSON assets.

---

### Task 1: Lock ambient behavior with failing tests

**Files:**
- Modify: `tests/audio/test_audio_state.cpp`
- Modify: `tests/audio/test_wav_audio_asset.cpp`
- Modify: `tests/audio/test_audio_asset_validator.py`

**Step 1: Write failing snapshot test**

Assert that registered `ambient_layers` resolve to concrete ambient asset paths in the snapshot.

**Step 2: Run test to verify it fails**

Run: `ctest --test-dir build --output-on-failure -R 'audio_state'`
Expected: FAIL because the snapshot has no resolved ambient tracks.

**Step 3: Write failing mix helper test**

Assert that a looping audio mix helper adds multiple assets into one target buffer and wraps cursors correctly.

**Step 4: Run test to verify it fails**

Run: `ctest --test-dir build --output-on-failure -R 'wav_audio_asset'`
Expected: FAIL because the helper does not exist yet.

**Step 5: Write failing validator test**

Assert that a missing ambient asset id causes validator failure.

**Step 6: Run test to verify it fails**

Run: `python3 tests/audio/test_audio_asset_validator.py`
Expected: FAIL because ambient assets are not validated yet.

### Task 2: Implement ambient resolution and runtime mixing

**Files:**
- Modify: `engine/audio/AudioBackend.h`
- Modify: `engine/audio/AudioDirector.cpp`
- Modify: `engine/audio/WavAudioAsset.h`
- Modify: `engine/audio/WavAudioAsset.cpp`
- Modify: `engine/audio/AudioBackendSDL.cpp`

**Step 1: Add `resolved_ambient_tracks` to the snapshot**

Keep logical ids in `ambient_layers`, but add resolved file paths for backend consumption.

**Step 2: Add a reusable looping mix helper**

Implement a helper that mixes one looping stereo asset into a float output buffer with a gain and cursor.

**Step 3: Mix ambient assets in the SDL backend**

Load up to two ambient assets from the resolved track list and mix them over real BGM or fallback tone output.

**Step 4: Run targeted tests**

Run: `ctest --test-dir build --output-on-failure -R 'audio_state|wav_audio_asset'`
Expected: PASS.

### Task 3: Add ambient demo assets and validation

**Files:**
- Create: `assets/audio/ambient/wind.wav`
- Create: `assets/audio/ambient/rumble.wav`
- Create: `assets/audio/ambient/water.wav`
- Modify: `tools/validate_assets.py`
- Modify: `tests/audio/test_audio_asset_validator.py`

**Step 1: Add ambient demo assets**

Generate 3 short loopable ambient `WAV` files.

**Step 2: Validate ambient ids**

Require each non-empty `ambient_layers` id to resolve to an existing `.wav` under `assets/audio/ambient/`.

**Step 3: Run validator regression**

Run: `python3 tests/audio/test_audio_asset_validator.py`
Expected: PASS.

### Task 4: Run full regression

**Step 1: Run full verification**

```bash
cmake -S . -B build -G Ninja
cmake --build build
ctest --test-dir build --output-on-failure
python3 tools/validate_assets.py
SDL_AUDIODRIVER=dummy SDL_VIDEODRIVER=dummy RESONANCE_DEMO_HIDDEN=1 RESONANCE_DEMO_AUTOCLOSE=1 ./build/resonance_demo
```

Expected: all commands succeed.
