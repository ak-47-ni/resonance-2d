#!/usr/bin/env python3
import json
import sys
from pathlib import Path


def load_json(path: Path):
    with path.open("r", encoding="utf-8") as handle:
        return json.load(handle)


def require_positive_bounds(region: dict) -> None:
    for key in ("x", "y", "width", "height"):
        if key not in region or not isinstance(region[key], (int, float)):
            raise SystemExit(f"Region '{region.get('id', '<unknown>')}' is missing numeric field '{key}'")
    if region["width"] <= 0 or region["height"] <= 0:
        raise SystemExit(f"Region '{region['id']}' must have positive width and height")


def resolve_data_root(argv: list[str]) -> Path:
    if len(argv) > 1:
        return Path(argv[1]).resolve()
    return (Path(__file__).resolve().parent.parent / "assets" / "data").resolve()


def require_wav_audio_assets(music_states: list[dict], assets_root: Path) -> None:
    for music_state in music_states:
        music_state_id = music_state.get("id", "<unknown>")
        bgm_track = music_state.get("bgm_track")
        if not isinstance(bgm_track, str) or not bgm_track:
            raise SystemExit(f"Music state '{music_state_id}' is missing string field 'bgm_track'")
        if Path(bgm_track).suffix.lower() != ".wav":
            raise SystemExit(f"Music state '{music_state_id}' must reference a .wav bgm_track")
        resolved_path = assets_root / bgm_track
        if not resolved_path.exists():
            raise SystemExit(f"Missing audio asset: {resolved_path}")

        ambient_layers = music_state.get("ambient_layers", [])
        if not isinstance(ambient_layers, list):
            raise SystemExit(f"Music state '{music_state_id}' must define ambient_layers as an array")
        for ambient_layer in ambient_layers:
            if not isinstance(ambient_layer, str) or not ambient_layer:
                raise SystemExit(f"Music state '{music_state_id}' contains an invalid ambient layer id")
            ambient_path = assets_root / "audio" / "ambient" / f"{ambient_layer}.wav"
            if not ambient_path.exists():
                raise SystemExit(f"Missing ambient asset: {ambient_path}")


def main(argv: list[str]) -> int:
    root = resolve_data_root(argv)
    assets_root = root.parent
    regions_path = root / "regions" / "regions.json"
    music_path = root / "music" / "music_states.json"
    events_path = root / "events" / "events.json"

    for path in (regions_path, music_path, events_path):
        if not path.exists():
            raise SystemExit(f"Missing asset data file: {path}")

    regions = load_json(regions_path)["regions"]
    music_states = load_json(music_path)["music_states"]
    events = load_json(events_path)["events"]

    region_ids = {item["id"] for item in regions}
    music_state_ids = {item["id"] for item in music_states}

    for region in regions:
        require_positive_bounds(region)
        if region["default_music_state"] not in music_state_ids:
            raise SystemExit(
                f"Region '{region['id']}' references missing music state '{region['default_music_state']}'"
            )

    for event in events:
        if event["region_id"] not in region_ids:
            raise SystemExit(
                f"Event '{event['id']}' references missing region '{event['region_id']}'"
            )
        if event["requested_music_state"] not in music_state_ids:
            raise SystemExit(
                f"Event '{event['id']}' references missing music state '{event['requested_music_state']}'"
            )

    require_wav_audio_assets(music_states, assets_root)

    print("Asset validation passed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv))
