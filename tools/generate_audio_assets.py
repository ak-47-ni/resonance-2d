#!/usr/bin/env python3
from __future__ import annotations

import argparse
import math
import wave
from dataclasses import dataclass
from pathlib import Path
from typing import Iterable


SAMPLE_RATE = 48_000
CHANNELS = 2
SAMPLE_WIDTH = 2


@dataclass(frozen=True)
class TrackPreset:
    group: str
    name: str
    duration_seconds: float
    left_frequency: float
    right_frequency: float
    amplitude: float
    modulation_frequency: float
    shimmer_amount: float

    @property
    def relative_path(self) -> Path:
        return Path(self.group) / f"{self.name}.wav"


PRESETS: tuple[TrackPreset, ...] = (
    TrackPreset("bgm", "explore", 1.25, 220.0, 330.0, 0.28, 0.50, 0.00),
    TrackPreset("bgm", "mysterious", 1.25, 164.81, 246.94, 0.28, 0.47, 0.00),
    TrackPreset("bgm", "calm", 1.25, 196.0, 293.66, 0.26, 0.42, 0.00),
    TrackPreset("ambient", "wind", 1.50, 120.0, 180.0, 0.10, 0.13, 0.15),
    TrackPreset("ambient", "rumble", 1.50, 55.0, 82.5, 0.12, 0.11, 0.08),
    TrackPreset("ambient", "water", 1.50, 240.0, 360.0, 0.08, 0.17, 0.12),
)


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Generate deterministic demo audio assets.")
    parser.add_argument(
        "--output-root",
        type=Path,
        default=Path("assets") / "audio",
        help="Target root for generated audio assets (default: assets/audio).",
    )
    parser.add_argument(
        "--group",
        choices=("all", "bgm", "ambient"),
        default="all",
        help="Generate all tracks or a single group.",
    )
    return parser.parse_args()


def selected_presets(group: str) -> Iterable[TrackPreset]:
    if group == "all":
        return PRESETS
    return tuple(preset for preset in PRESETS if preset.group == group)


def clamp_unit(sample: float) -> float:
    return max(-1.0, min(1.0, sample))


def to_pcm16(sample: float) -> bytes:
    return int(clamp_unit(sample) * 32767.0).to_bytes(2, "little", signed=True)


def synthesize_frame(preset: TrackPreset, time_seconds: float) -> tuple[float, float]:
    drift = 0.75 + 0.25 * math.sin(2.0 * math.pi * preset.modulation_frequency * time_seconds)
    left = math.sin(2.0 * math.pi * preset.left_frequency * time_seconds) * preset.amplitude * drift
    right = math.sin(2.0 * math.pi * preset.right_frequency * time_seconds) * preset.amplitude * drift

    if preset.shimmer_amount > 0.0:
        shimmer = math.sin(2.0 * math.pi * (preset.left_frequency * 0.25) * time_seconds)
        left += shimmer * preset.amplitude * preset.shimmer_amount
        right -= shimmer * preset.amplitude * preset.shimmer_amount * 0.8

    return left, right


def write_track(output_root: Path, preset: TrackPreset) -> Path:
    target_path = output_root / preset.relative_path
    target_path.parent.mkdir(parents=True, exist_ok=True)

    frame_count = int(SAMPLE_RATE * preset.duration_seconds)
    frames = bytearray()
    for index in range(frame_count):
        time_seconds = index / SAMPLE_RATE
        left, right = synthesize_frame(preset, time_seconds)
        frames.extend(to_pcm16(left))
        frames.extend(to_pcm16(right))

    with wave.open(str(target_path), "wb") as handle:
        handle.setnchannels(CHANNELS)
        handle.setsampwidth(SAMPLE_WIDTH)
        handle.setframerate(SAMPLE_RATE)
        handle.writeframes(bytes(frames))

    return target_path


def main() -> int:
    args = parse_args()
    output_root: Path = args.output_root.resolve()
    presets = tuple(selected_presets(args.group))
    if not presets:
        raise SystemExit(f"No presets matched group '{args.group}'")

    written_paths = [write_track(output_root, preset) for preset in presets]
    for written_path in written_paths:
        print(written_path.relative_to(output_root).as_posix())
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
