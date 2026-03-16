# Hidden Story Route Design

## Goal

让 `resonance_demo` 在 hidden 模式下支持一条脚本化的 memory-chain 巡检路线，用终端输出快速验证故事锚点、记忆链和站台事件升级是否连通。

## Options

### Option A — 在现有 `resonance_demo` 中增加环境变量驱动的 scripted route（推荐）

- 通过例如 `RESONANCE_DEMO_ROUTE=memory_chain` 启用
- hidden 模式下自动移动到既定坐标、触发交互，并输出每一步 summary
- 不新增 target，不改 `CMakeLists.txt`，直接复用现有 demo 和 hidden runtime 验证方式

### Option B — 新建独立 CLI debug 工具

- 优点：职责清晰
- 缺点：需要新增 target 与构建配置，当前与 Terminal B 对 `CMakeLists.txt` 的占用相冲突

### Option C — 只做单元测试里的路线 helper，不接入真实 demo

- 优点：实现最轻
- 缺点：对手动 smoke 和终端侧验收帮助有限，价值不如直接接到 demo

## Chosen Approach

采用 **Option A**，并配一个轻量 header-only route helper 供 `main.cpp` 与现有 demo 测试共享。

## Behavior

- 默认行为不变
- 当 `RESONANCE_DEMO_HIDDEN=1` 且 `RESONANCE_DEMO_ROUTE=memory_chain` 时：
  - 初始化 `DemoScene`
  - 顺序执行 meadow / ruins / lakeside / station 的关键巡检节点
  - 在每个节点输出一行带标签的 `debug_summary()`
- 输出应覆盖：
  - 初始 meadow 状态
  - `meadow-swing` 记忆解锁
  - `ruins-gate` 后站台升级到 `platform_convergence`
  - `lakeside-reeds` 后站台升级到 `terminal_refrain`

## Scope

- `game/demo/main.cpp`
- 新增 header-only route helper
- 扩展现有 demo 测试与文档

## Out of Scope

- 新的 JSON 路线配置
- 任意脚本语言嵌入
- 新的可执行文件或构建目标
