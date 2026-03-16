# Audio Ambient Layers Phase 2 Design

## Goal

在已打通真实 `WAV` BGM 的基础上，为 `music_state` 增加最多 2 层真实 `ambient` 循环叠加；缺失 ambient 资源时忽略该层，不中断 BGM 或 fallback。

## Scope

- 主要改动限制在 `engine/audio/`、`assets/audio/ambient/`、`tools/validate_assets.py`、`tests/audio/`
- 不改 `world/event` 数据结构
- 不做自动 ducking、淡入淡出、优先级调度或 region-level ambient 合并

## Chosen Approach

### Option A — Snapshot-resolved ambient tracks

- `AudioDirector` 继续负责将 `ambient_layers` 中的 layer id 解析成 `assets/audio/ambient/<layer>.wav`
- `AudioStateSnapshot` 新增 `resolved_ambient_tracks`
- `AudioBackendSDL` 在单一输出流中混合：真实 BGM / tone fallback + 最多 2 条 ambient asset

### Why this option

- 保持播放层不理解内容层约定
- 继续复用当前 SDL stream 架构
- 改动小，便于和主线并行

## Runtime Rules

- 最多混 2 条 ambient 资产；超出部分在 backend 截断
- 若真实 BGM 可用：输出 `BGM + ambient`
- 若真实 BGM 缺失：输出 `tone fallback + ambient`
- 若某条 ambient 缺失：仅忽略该层

## Validation

- `tools/validate_assets.py` 校验 `music_states.json` 中每个 ambient layer id 对应的 `assets/audio/ambient/<id>.wav` 是否存在
- 不接受空字符串 ambient layer id

## Testing

- 扩展 `tests/audio/test_audio_state.cpp`，验证 `resolved_ambient_tracks`
- 扩展 `tests/audio/test_wav_audio_asset.cpp`，验证循环混音 helper 的叠加与游标回绕
- 扩展 `tests/audio/test_audio_asset_validator.py`，验证缺失 ambient 资产被拒绝
