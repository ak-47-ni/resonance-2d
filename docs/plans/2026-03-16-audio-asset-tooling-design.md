# Audio Asset Tooling Phase 3 Design

## Goal

提供一个零依赖、可重复执行的 Python 工具，用一条命令重建当前 demo 音频资产，避免仓库对“手工制作二进制音频文件”的隐式依赖。

## Scope

- 新增 `tools/generate_audio_assets.py`
- 新增面向工具的测试
- 可选地把该测试接入 `ctest`
- 不修改 runtime 资源路径约定，不引入外部音频处理依赖

## Chosen Approach

### Option A — Built-in preset generator

在脚本内部维护一份最小 preset 清单，直接用 Python 标准库 `wave` + `math` 生成：

- `assets/audio/bgm/explore.wav`
- `assets/audio/bgm/mysterious.wav`
- `assets/audio/bgm/calm.wav`
- `assets/audio/ambient/wind.wav`
- `assets/audio/ambient/rumble.wav`
- `assets/audio/ambient/water.wav`

### Why this option

- 不引入第三方依赖
- 适合当前仓库的轻量工具风格
- 最容易测试，也最容易在新机器上重建 demo 资产

## CLI

默认命令：

```bash
python3 tools/generate_audio_assets.py
```

支持两个参数：

- `--output-root <path>`，默认 `assets/audio`
- `--group all|bgm|ambient`，默认 `all`

## Output Contract

所有输出文件固定为：

- `48000 Hz`
- 双声道
- `16-bit PCM WAV`

脚本默认覆盖写入，保证结果可重复。

## Error Handling

- 非法 `--group` 返回非零
- 输出目录无法创建或文件无法写入时返回非零
- 不部分成功静默退出；若任一文件失败，脚本应显式报错

## Testing

新增 `tests/audio/test_generate_audio_assets.py`：

- 在临时目录运行生成脚本
- 校验 `all`、`bgm`、`ambient` 三种模式
- 用 Python `wave` 验证生成文件的采样率、声道数、帧数
- 校验脚本输出的文件集合和预期一致

## Integration

- 可将该 Python 测试接入 `ctest`，保证全量回归时自动覆盖工具链
- `validate_assets.py` 不负责生成，只继续校验仓库内资产的存在性与结构
