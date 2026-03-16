# Journal Context Design

## Goal

让 memory journal 在不改变核心玩法的前提下，显示每条记忆所属的 `region`，并补一份长期维护的 runtime-context 文档，降低长对话后丢上下文的风险。

## Options

### Option A — 在 journal overlay 中显示 `Region:`（推荐）

- 直接复用 `MemoryJournalEntry.region_id`
- 每条 journal 记录显示 `Entry`、`Region`、`Memory`
- 只改展示层，不改事件/音频/内容结构

### Option B — 给 region 做更友好的展示名

- 更好看
- 但需要引入额外映射或内容字段，这一刀没必要

### Option C — 只补文档，不改运行态

- 风险最低
- 但 journal 本身仍缺少上下文，试玩/验收时阅读成本高

## Chosen Approach

采用 **Option A**，并同步新增 `docs/runtime-context.md` 作为当前分支的运行态快照。

## Behavior

- 触发 story anchor 后写入 journal 的行为保持不变
- Journal 视图现在对每条记录额外显示一行 `Region: <region_id>`
- `memory_journal_entries()` 中的 `region_id` 继续作为运行态数据被测试覆盖
- `docs/runtime-context.md` 记录：当前分支、共享看板、已验证功能、关键命令、文件 ownership 和下一步建议

## Out of Scope

- region 本地化展示名
- journal 分类/筛选
- 持久化存档
- 新的构建目标或 SDL UI 重构
