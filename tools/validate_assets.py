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


def require_positive_radius(anchor: dict) -> None:
    if "activation_radius" not in anchor or not isinstance(anchor["activation_radius"], (int, float)):
        raise SystemExit(f"Story anchor '{anchor.get('id', '<unknown>')}' is missing numeric field 'activation_radius'")
    if anchor["activation_radius"] <= 0:
        raise SystemExit(f"Story anchor '{anchor['id']}' must have positive activation_radius")


def require_non_empty_string(item: dict, item_type: str, key: str) -> None:
    value = item.get(key)
    if not isinstance(value, str) or not value.strip():
        raise SystemExit(f"{item_type} '{item.get('id', '<unknown>')}' must define non-empty '{key}'")


def require_string_array(item: dict, item_type: str, key: str) -> None:
    value = item.get(key)
    if value is None:
        return
    if not isinstance(value, list) or any((not isinstance(entry, str) or not entry.strip()) for entry in value):
        raise SystemExit(f"{item_type} '{item.get('id', '<unknown>')}' must define '{key}' as a non-empty string array")


def main(argv: list[str]) -> int:
    root = resolve_data_root(argv)
    assets_root = root.parent
    regions_path = root / "regions" / "regions.json"
    music_path = root / "music" / "music_states.json"
    events_path = root / "events" / "events.json"
    story_path = root / "story" / "story_anchors.json"

    for path in (regions_path, music_path, events_path, story_path):
        if not path.exists():
            raise SystemExit(f"Missing asset data file: {path}")

    regions = load_json(regions_path)["regions"]
    music_states = load_json(music_path)["music_states"]
    events = load_json(events_path)["events"]
    story_anchors = load_json(story_path)["story_anchors"]

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
        require_string_array(event, "Event", "required_world_tags")

    for anchor in story_anchors:
        require_non_empty_string(anchor, "Story anchor", "region_id")
        require_non_empty_string(anchor, "Story anchor", "prompt_text")
        require_non_empty_string(anchor, "Story anchor", "story_text")
        require_positive_radius(anchor)
        if anchor["region_id"] not in region_ids:
            raise SystemExit(
                f"Story anchor '{anchor['id']}' references missing region '{anchor['region_id']}'"
            )

    require_wav_audio_assets(music_states, assets_root)

    print("Asset validation passed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv))
