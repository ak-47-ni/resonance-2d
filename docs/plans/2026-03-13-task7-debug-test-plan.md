# Task 7 Debug Overlay Test Plan

## Summary

- Feature/Change: Add a rolling `TraceLog` plus a lightweight `DebugOverlay` text formatter for region, music, and recent events.
- Module/Area: `engine/debug/TraceLog`, `engine/debug/DebugOverlay`.
- Scope: Data retention and formatted overlay lines only.

## Test Mapping (1:1)

| Code Change | File/Function | Test Change | Test File |
| --- | --- | --- | --- |
| Add rolling trace log | `engine/debug/TraceLog.*` | Verify the log keeps only the most recent entries | `tests/debug/test_trace_log.cpp` |
| Add overlay formatter | `engine/debug/DebugOverlay.*` | Build target compiles against the formatter surface | `tests/debug/test_trace_log.cpp` |

## Test Cases

- Happy path: when a fourth item is pushed into a three-entry log, the oldest item is dropped.
- Edge cases: zero entries yields an empty overlay history section.
- Error cases: empty strings are allowed without crashing.

## Test Execution

- Command(s): `cmake --build build && ctest --test-dir build -R trace_log --output-on-failure`
- Environment: local macOS shell.
- Data fixtures: none.
- Expected duration: under 10 seconds.

## Risks

- Known flakiness: none.
- External dependencies: none.
- Follow-up work: bind overlay text to actual on-screen debug rendering later.
