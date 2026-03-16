#!/usr/bin/env python3
import json
import subprocess
import sys
import tempfile
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[2]
SCRIPT = REPO_ROOT / "tools" / "validate_assets.py"


def write_json(path: Path, payload: dict) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(payload, indent=2), encoding="utf-8")


def write_wav(path: Path) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_bytes(b"RIFFdemoWAVEfmt ")


def build_tree(root: Path, *, bgm_track: str, ambient_layers: list[str]) -> None:
    assets_root = root / "assets"
    data_root = assets_root / "data"

    write_json(
        data_root / "regions" / "regions.json",
        {
            "regions": [
                {
                    "id": "meadow",
                    "x": 0,
                    "y": 0,
                    "width": 10,
                    "height": 10,
                    "default_music_state": "explore",
                    "ambient_layers": [],
                }
            ]
        },
    )
    write_json(
        data_root / "music" / "music_states.json",
        {
            "music_states": [
                {
                    "id": "explore",
                    "bgm_track": bgm_track,
                    "ambient_layers": ambient_layers,
                }
            ]
        },
    )
    write_json(
        data_root / "events" / "events.json",
        {
            "events": [
                {
                    "id": "welcome",
                    "region_id": "meadow",
                    "requested_music_state": "explore",
                    "weight": 1,
                }
            ]
        },
    )


def run_validator(asset_data_root: Path) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        [sys.executable, str(SCRIPT), str(asset_data_root)],
        check=False,
        capture_output=True,
        text=True,
    )


def main() -> int:
    with tempfile.TemporaryDirectory() as temp_dir:
        temp_root = Path(temp_dir)
        asset_data_root = temp_root / "assets" / "data"

        build_tree(temp_root, bgm_track="audio/bgm/explore.mp3", ambient_layers=["wind"])
        invalid_ext = run_validator(asset_data_root)
        invalid_ext_ok = invalid_ext.returncode != 0 and ".wav" in (invalid_ext.stderr + invalid_ext.stdout)

        build_tree(temp_root, bgm_track="audio/bgm/explore.wav", ambient_layers=["wind"])
        missing_file = run_validator(asset_data_root)
        missing_file_ok = missing_file.returncode != 0 and "Missing audio asset" in (missing_file.stderr + missing_file.stdout)

        write_wav(temp_root / "assets" / "audio" / "bgm" / "explore.wav")
        missing_ambient = run_validator(asset_data_root)
        missing_ambient_ok = missing_ambient.returncode != 0 and "Missing ambient asset" in (missing_ambient.stderr + missing_ambient.stdout)

    return 0 if invalid_ext_ok and missing_file_ok and missing_ambient_ok else 1


if __name__ == "__main__":
    raise SystemExit(main())
