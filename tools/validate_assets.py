#!/usr/bin/env python3
import json
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


def main() -> int:
    root = Path(__file__).resolve().parent.parent / "assets" / "data"
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

    print("Asset validation passed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
