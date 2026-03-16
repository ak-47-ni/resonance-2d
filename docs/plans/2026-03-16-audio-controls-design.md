# Audio Controls Phase 4 Design

## Goal

为当前音频播放链路增加第一版全局控制能力：`master_volume`、`bgm_volume`、`ambient_volume` 与 `crossfade_seconds`，让 BGM 切换具备平滑过渡，同时保持现有 WAV 资产加载与 tone fallback 逻辑可用。

## Scope

- 扩展音频快照结构，携带全局控制参数
- 在 SDL audio backend 中实现 BGM crossfade
- 让 BGM、ambient、tone fallback 都受全局音量控制影响
- 为快照透传与 backend 行为补自动化测试
- 不修改 demo 内容数据、配置文件或 UI 暴露方式

## Non-Goals

- 不在本阶段把控制项暴露到 `assets/data/*`
- 不新增 debug 菜单、运行时按键或可视化控制面板
- 不为 ambient 层实现 crossfade；环境层仍按快照直接切换
- 不重构为独立 mixer/transport 子系统

## Chosen Approach

### Option A — Backend-owned global controls

把全局控制值挂在 `AudioStateSnapshot` 上，由 `AudioBackendSDL` 直接消费并实现：

- BGM 输出音量
- ambient 输出音量
- tone fallback 输出音量
- BGM 轨道切换时的 crossfade

### Why this option

- 改动集中在 Terminal B 已 claim 的音频 backend 与音频测试
- 不需要在这一版扩大 `AudioDirector` 以外的运行时集成面
- 后续若要从 config/content/debug 注入控制值，只需继续复用同一个快照字段

## Data Contract

在 `AudioStateSnapshot` 中新增 4 个字段：

- `master_volume`：最终总输出增益，默认 `1.0`
- `bgm_volume`：BGM 与 tone fallback 的轨道增益，默认 `1.0`
- `ambient_volume`：ambient 轨道总增益，默认 `1.0`
- `crossfade_seconds`：BGM 切换交叉淡化时长，默认 `1.0`

所有值在 backend 中统一 clamp：

- 音量值范围 `[0.0, 1.0]`
- `crossfade_seconds < 0.0` 时按 `0.0` 处理

## Runtime Behavior

### BGM

- 当 `resolved_bgm_track` 变化时，若旧轨与新轨不同，则启动 crossfade
- crossfade 期间同时混合“旧主轨”和“新主轨”
- 若 `crossfade_seconds == 0.0`，则退化为立即切换
- 若新旧轨道相同，则不重置游标，不触发 crossfade，只更新音量控制

### Tone fallback

- 当主 BGM WAV 资产不可用时，继续使用现有 `AudioToneProfile`
- tone fallback 视为“主轨输出”的一部分，因此受 `bgm_volume * master_volume` 控制
- 第一版不对 tone fallback 做独立 crossfade，只保持与当前快照一致的最小行为

### Ambient

- ambient 层继续沿用现有的多层 looping 逻辑
- 快照变化时直接切换到新环境层集合
- 最终增益为：现有层基准增益 × `ambient_volume` × `master_volume`

## State Model

`AudioBackendSDL` 需要维护两类主轨状态：

- 当前主轨：当前应持续播放的 BGM WAV 或 tone fallback
- 过渡主轨：crossfade 尚未结束时，保留上一条主轨及其读取游标

推荐保留以下内部概念：

- 当前轨道路径 / 资产 / 游标
- 过渡中旧轨路径 / 资产 / 游标
- crossfade 已推进时长
- crossfade 总时长
- 当前快照的有效控制值（已 clamp）

## Error Handling

- BGM WAV 缺失或解析失败时，不报错中断，继续走 tone fallback
- ambient WAV 缺失时，缺失层静默跳过，保持其余层可播放
- SDL stream 初始化失败时保持当前退化行为，不新增异常路径
- 任何控制值异常都在 backend 内部 clamp，不把非法值传播到混音阶段

## Testing

### 1. Snapshot contract test

扩展 `tests/audio/test_audio_state.cpp`，校验：

- 默认控制字段存在且为预期默认值
- music state / ambient layer 变化时，控制字段仍稳定透传

### 2. Backend behavior test

新增聚焦测试，建议命名为 `tests/audio/test_audio_backend_controls.cpp`，覆盖：

- 不同控制值会影响 backend 暴露的内部/可观察状态
- `crossfade_seconds == 0.0` 时立即切换
- 新旧轨相同不触发 crossfade
- 轨道变化时进入 crossfade，并按推进时间逐步完成
- 音量值 clamp 后仍满足预期边界

为保持测试可控，backend 需要提供最小测试钩子或可替代的无 SDL 驱动执行路径，而不是依赖真实音频设备。

## Verification

最小验证集：

```bash
cmake -S . -B build -G Ninja
cmake --build build
ctest --test-dir build --output-on-failure -R 'audio_state|audio_backend_controls|wav_audio_asset'
```

全量验证集：

```bash
ctest --test-dir build --output-on-failure
python3 tools/validate_assets.py
SDL_AUDIODRIVER=dummy SDL_VIDEODRIVER=dummy RESONANCE_DEMO_HIDDEN=1 RESONANCE_DEMO_AUTOCLOSE=1 ./build/resonance_demo
```
