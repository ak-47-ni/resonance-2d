# Audio Asset Tooling Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Add a deterministic Python tool that regenerates all demo audio assets, and cover it with automated tests.

**Architecture:** A single Python CLI script owns a built-in preset manifest for demo BGM and ambient tracks, writes stereo 48 kHz PCM WAV files to a target root, and is validated by a Python regression test that runs in a temporary directory. The runtime and validator continue consuming the generated files without path changes.

**Tech Stack:** Python 3 standard library (`argparse`, `math`, `pathlib`, `wave`, `tempfile`, `subprocess`), existing CMake/CTest project verification.

---

### Task 1: Lock generator behavior with failing tests

**Files:**
- Create: `tests/audio/test_generate_audio_assets.py`
- Modify: `CMakeLists.txt`

**Step 1: Write the failing Python test**

Create a temporary output root and assert that the generator can produce the expected `bgm` and `ambient` file sets, with valid WAV metadata.

**Step 2: Run test to verify it fails**

Run: `python3 tests/audio/test_generate_audio_assets.py`
Expected: FAIL because `tools/generate_audio_assets.py` does not exist yet.

**Step 3: Wire test into `ctest`**

Add an `audio_asset_generator` test entry so the tool is covered by the standard regression suite.

**Step 4: Run `ctest` target to verify it fails**

Run: `ctest --test-dir build --output-on-failure -R 'audio_asset_generator'`
Expected: FAIL because the generator script does not exist yet.

### Task 2: Implement the generator

**Files:**
- Create: `tools/generate_audio_assets.py`
- Test: `tests/audio/test_generate_audio_assets.py`

**Step 1: Add a built-in preset manifest**

Define deterministic presets for the current 3 BGM and 3 ambient files.

**Step 2: Add WAV synthesis helpers**

Generate stereo 16-bit PCM samples at 48 kHz with small waveform variations per preset.

**Step 3: Add CLI argument parsing**

Support `--output-root` and `--group all|bgm|ambient`.

**Step 4: Run generator test to verify it passes**

Run: `python3 tests/audio/test_generate_audio_assets.py`
Expected: PASS.

### Task 3: Run regression

**Files:**
- Modify: `docs/plans/2026-03-16-audio-asset-tooling-design.md`
- Modify: `docs/plans/2026-03-16-audio-asset-tooling-implementation.md`

**Step 1: Run focused verification**

Run: `ctest --test-dir build --output-on-failure -R 'audio_asset_generator|audio_state|wav_audio_asset'`
Expected: PASS.

**Step 2: Run full verification**

```bash
cmake -S . -B build -G Ninja
cmake --build build
ctest --test-dir build --output-on-failure
python3 tools/validate_assets.py
SDL_AUDIODRIVER=dummy SDL_VIDEODRIVER=dummy RESONANCE_DEMO_HIDDEN=1 RESONANCE_DEMO_AUTOCLOSE=1 ./build/resonance_demo
```

Expected: all commands succeed.
