#!/usr/bin/env python3
from __future__ import annotations

import importlib.util
import json
import tempfile
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[2]
SCRIPT = REPO_ROOT / "tools" / "validate_assets.py"


def load_validator_module():
    spec = importlib.util.spec_from_file_location("validate_assets", SCRIPT)
    if spec is None or spec.loader is None:
        raise RuntimeError(f"Unable to load validator module from {SCRIPT}")
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


def write_json(path: Path, payload: dict) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(payload, indent=2), encoding="utf-8")


def write_wav(path: Path) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_bytes(b"RIFFdemoWAVEfmt ")


def build_tree(
    root: Path,
    *,
    regions: list[dict] | None = None,
    music_states: list[dict] | None = None,
    events: list[dict] | None = None,
    story_anchors: list[dict] | None = None,
) -> Path:
    assets_root = root / "assets"
    data_root = assets_root / "data"

    write_json(
        data_root / "regions" / "regions.json",
        {
            "regions": regions if regions is not None else [
                {
                    "id": "meadow",
                    "x": 0,
                    "y": 0,
                    "width": 10,
                    "height": 10,
                    "default_music_state": "explore",
                    "ambient_layers": ["wind"],
                }
            ]
        },
    )
    write_json(
        data_root / "music" / "music_states.json",
        {
            "music_states": music_states if music_states is not None else [
                {
                    "id": "explore",
                    "bgm_track": "audio/bgm/explore.wav",
                    "ambient_layers": ["wind"],
                }
            ]
        },
    )
    write_json(
        data_root / "events" / "events.json",
        {
            "events": events if events is not None else [
                {
                    "id": "welcome",
                    "region_id": "meadow",
                    "requested_music_state": "explore",
                    "weight": 1,
                }
            ]
        },
    )
    write_json(
        data_root / "story" / "story_anchors.json",
        {
            "story_anchors": story_anchors if story_anchors is not None else [
                {
                    "id": "meadow-memory",
                    "region_id": "meadow",
                    "x": 2,
                    "y": 3,
                    "prompt_text": "Listen closely",
                    "story_text": "The meadow hums with distant echoes.",
                    "activation_radius": 12,
                }
            ]
        },
    )

    write_wav(assets_root / "audio" / "bgm" / "explore.wav")
    write_wav(assets_root / "audio" / "ambient" / "wind.wav")
    return data_root


def expect_system_exit(callable_obj, expected_substring: str) -> bool:
    try:
        callable_obj()
    except SystemExit as exc:
        return expected_substring in str(exc)
    return False


def main() -> int:
    validator = load_validator_module()
    validate_data_root = getattr(validator, "validate_data_root", None)
    if validate_data_root is None:
        raise SystemExit("validate_data_root is missing")

    with tempfile.TemporaryDirectory() as temp_dir:
        temp_root = Path(temp_dir)

        valid_root = build_tree(temp_root / "valid")
        validate_data_root(valid_root)

        duplicate_region_root = build_tree(
            temp_root / "duplicate_region",
            regions=[
                {
                    "id": "meadow",
                    "x": 0,
                    "y": 0,
                    "width": 10,
                    "height": 10,
                    "default_music_state": "explore",
                    "ambient_layers": ["wind"],
                },
                {
                    "id": "meadow",
                    "x": 12,
                    "y": 0,
                    "width": 8,
                    "height": 8,
                    "default_music_state": "explore",
                    "ambient_layers": ["wind"],
                },
            ],
        )
        duplicate_region_ok = expect_system_exit(
            lambda: validate_data_root(duplicate_region_root),
            "Duplicate Region id",
        )

        invalid_weight_root = build_tree(
            temp_root / "invalid_weight",
            events=[
                {
                    "id": "welcome",
                    "region_id": "meadow",
                    "requested_music_state": "explore",
                    "weight": 0,
                }
            ],
        )
        invalid_weight_ok = expect_system_exit(
            lambda: validate_data_root(invalid_weight_root),
            "positive integer 'weight'",
        )

    return 0 if duplicate_region_ok and invalid_weight_ok else 1


if __name__ == "__main__":
    raise SystemExit(main())
