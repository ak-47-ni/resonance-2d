# Audio Event Mix Runtime Design

## Goal

把已经存在于事件数据里的 `event_duck` / `ambient_boost` 真正接入运行态音频快照，让不同叙事事件不只是“被显示出来”，而是真的改变最终混音强度。

## Options

### Option A — 在 `AudioDirector` 中消费事件 mix 标量（推荐）

- `DemoScene` 在选中事件后，把当前事件的 `event_duck` / `ambient_boost` 传给 `AudioDirector`
- `AudioDirector` 在 `refresh_snapshot()` 中，把 story focus、event emphasis、event mix 三者合成到最终 `bgm_gain` / `ambient_gain_multiplier`
- `AudioBackendSDL` 继续只消费最终 snapshot gain，不感知事件数据结构

### Option B — 让 `AudioBackendSDL` 直接理解事件 mix profile

- 能更直接地在 backend 里操作
- 但会把事件域耦合进 backend，破坏当前分层

### Option C — 只在 debug overlay 显示，不改变最终音频快照

- 改动最小
- 但无法兑现“音乐沉浸感”的主目标

## Chosen Approach

采用 **Option A**。

## Behavior

- 没有事件或事件 profile 为默认值时，行为接近当前实现
- `event_duck < 1.0` 时，事件 emphasis 对 BGM 的压低会更强
- `ambient_boost < 1.0` 时，事件 emphasis 对环境层的抬升会更弱；更高值则更强
- 运行态调试输出新增最终解析后的 `BgmGain` / `AmbientGain`，便于 hidden demo 和测试验证

## Scope

- `engine/audio/AudioDirector.*`
- `game/demo/DemoScene.*`
- `engine/debug/DebugOverlay.*`
- `tests/audio/test_audio_state.cpp`
- `tests/demo/test_event_audio_emphasis.cpp`
- docs / runtime snapshot

## Out of Scope

- 新的音频资产格式
- 重新设计 SDL 混音器
- 配置菜单与用户滑杆
- 多事件叠加混音规则
