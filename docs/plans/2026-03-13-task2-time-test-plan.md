# Task 2 Time Core Test Plan

## Summary

- Feature/Change: Add a minimal time accumulator and wire the application to advance frames in headless mode.
- Module/Area: `engine/core/Time`, `engine/core/Application`.
- Scope: Only time stepping and a lightweight frame loop surface.

## Test Mapping (1:1)

| Code Change | File/Function | Test Change | Test File |
| --- | --- | --- | --- |
| Add time accumulator | `engine/core/Time.h`, `engine/core/Time.cpp` | Verify delta and total time are updated monotonically | `tests/core/test_time.cpp` |
| Wire application frame stepping | `engine/core/Application.cpp` | Rebuild startup smoke and time test against the updated core | `tests/smoke/test_startup.cpp`, `tests/core/test_time.cpp` |

## Test Cases

- Happy path: ticking twice updates `delta_seconds` and increases `total_seconds`.
- Edge cases: zero delta is accepted without producing negative totals.
- Error cases: missing time class should fail compilation during the RED step.

## Test Execution

- Command(s): `c++ -std=c++20 -I. tests/core/test_time.cpp -o /tmp/resonance_task2_time`
- Environment: macOS local shell, no SDL dependency required.
- Data fixtures: none.
- Expected duration: under 5 seconds.

## Risks

- Known flakiness: none.
- External dependencies: `cmake` remains unavailable on `PATH`, so direct compilation continues for the TDD loop.
- Follow-up work: replace manual compile commands with `ctest` once `cmake` is installed.
