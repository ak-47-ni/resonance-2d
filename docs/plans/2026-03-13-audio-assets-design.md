# Audio Assets Phase 1 Design

## Goal

让 `music_state` 在不改动主线 runtime 结构的前提下，驱动真实 `WAV` BGM 播放；当资源不可用或音频后端不可用时，继续回退到现有 `AudioToneProfile` 合成音。

## Scope

- 主要改动限制在 `engine/audio/`、`assets/audio/`、`assets/data/music/`、`tools/validate_assets.py`、`tests/audio/`
- 只在 `game/demo/DemoScene.cpp` 做最小接线，把 `bundle.music_states` 注册给 `AudioDirector`
- 本阶段不做 `ambient_layers` 混音，不引入 `SDL_mixer` 或额外解码依赖

## Chosen Approach

### Option A — Director-resolved playback snapshot

由 `AudioDirector` 持有 `MusicStateData` 映射，并把逻辑态解析成更完整的 `AudioStateSnapshot`：

- 保留逻辑字段 `music_state`
- 新增 `resolved_bgm_track`
- 新增 `fallback_music_state`
- 保留 `ambient_layers`

`AudioBackendSDL` 只消费解析后的快照；它不回头读 JSON，也不理解内容层结构。

### Why this option

- 保持逻辑层和播放层职责清晰
- 改动集中，和主线 `world/event` 冲突最小
- 允许 backend 在资源缺失时直接退回 tone fallback
- 即使当前环境没有 SDL，也能通过快照解析与内容校验测试验证主要行为

## Runtime Flow

1. `load_demo_content()` 继续加载 `music_states.json`
2. `DemoScene(bundle)` 在构造时把 `bundle.music_states` 注册给 `AudioDirector`
3. `AudioDirector::refresh_snapshot()` 根据当前逻辑态解析出 `resolved_bgm_track`
4. `AudioBackendSDL::apply()` 保存快照并准备两条播放路径：
   - 真实 `WAV` 资产可用：播放 PCM buffer
   - 资源缺失 / 解码失败 / SDL 不可用：退回 `AudioToneProfile`

## Data Decisions

- `assets/data/music/music_states.json` 的 demo 轨道统一切到 `.wav`
- demo 先提供 3 条短循环 `WAV`：`explore`、`mysterious`、`calm`
- `ambient_layers` 继续保留在 JSON 和快照里，但本阶段只作为未来扩展字段，不参与真实混音

## Error Handling

- `AudioDirector` 解析不到已注册状态时，继续输出逻辑状态，但不提供 `resolved_bgm_track`
- `AudioBackendSDL` 加载文件失败时不抛出到外层，也不破坏音频流状态
- `tools/validate_assets.py` 增加两类失败：
  - `bgm_track` 文件不存在
  - `bgm_track` 扩展名不是 `.wav`

## Testing Strategy

- 新增 `AudioDirector` 解析测试：已注册状态能解析到正确的 `resolved_bgm_track`
- 新增 fallback 测试：缺失资源路径仍能生成 fallback 快照，不崩溃
- 新增 validator 回归测试：识别缺失文件和错误扩展名
- 保持现有全量 CMake/CTest 和 hidden demo 回归

## Notes

- 当前执行环境未检测到 SDL CMake 包，因此真实播放路径主要通过代码审查和非 SDL 测试保证正确性；最终仍需在带 SDL 的环境复核真实听感。
- 根据当前会话约束，不在这里执行 `git commit`。
