#!/usr/bin/env python3
from __future__ import annotations
import subprocess
import sys
import tempfile
import wave
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[2]
SCRIPT = REPO_ROOT / "tools" / "generate_audio_assets.py"

EXPECTED_BGM = {
    "bgm/explore.wav",
    "bgm/mysterious.wav",
    "bgm/calm.wav",
}
EXPECTED_AMBIENT = {
    "ambient/wind.wav",
    "ambient/rumble.wav",
    "ambient/water.wav",
}


def run_generator(output_root: Path, group: str) -> subprocess.CompletedProcess:
    return subprocess.run(
        [sys.executable, str(SCRIPT), "--output-root", str(output_root), "--group", group],
        check=False,
        capture_output=True,
        text=True,
    )


def collect_wavs(root: Path) -> set[str]:
    return {
        path.relative_to(root).as_posix()
        for path in root.rglob("*.wav")
    }


def assert_wave_ok(path: Path) -> bool:
    with wave.open(str(path), "rb") as handle:
        return (
            handle.getnchannels() == 2 and
            handle.getframerate() == 48000 and
            handle.getsampwidth() == 2 and
            handle.getnframes() > 0
        )


def main() -> int:
    with tempfile.TemporaryDirectory() as temp_dir:
        output_root = Path(temp_dir) / "audio"

        all_result = run_generator(output_root, "all")
        if all_result.returncode != 0:
            return 1
        all_wavs = collect_wavs(output_root)
        if all_wavs != (EXPECTED_BGM | EXPECTED_AMBIENT):
            return 1
        if not all(assert_wave_ok(output_root / relative_path) for relative_path in all_wavs):
            return 1

        bgm_only_root = Path(temp_dir) / "bgm-only"
        bgm_result = run_generator(bgm_only_root, "bgm")
        if bgm_result.returncode != 0 or collect_wavs(bgm_only_root) != EXPECTED_BGM:
            return 1

        ambient_only_root = Path(temp_dir) / "ambient-only"
        ambient_result = run_generator(ambient_only_root, "ambient")
        if ambient_result.returncode != 0 or collect_wavs(ambient_only_root) != EXPECTED_AMBIENT:
            return 1

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
