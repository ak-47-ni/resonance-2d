# Task 1 Bootstrap Test Plan

## Summary

- Feature/Change: Bootstrap the `resonance-2d` project with a minimal application skeleton and build system.
- Module/Area: `core`, demo entry point, smoke test, build scaffolding.
- Scope: Only the initial project skeleton for `Task 1`.

## Test Mapping (1:1)

| Code Change | File/Function | Test Change | Test File |
| --- | --- | --- | --- |
| Add minimal application type | `engine/core/Application.h` | Verify app can be constructed and reports headless readiness | `tests/smoke/test_startup.cpp` |
| Add bootstrap build targets | `CMakeLists.txt` | Ensure test target can be compiled from the public headers | `tests/smoke/test_startup.cpp` |

## Test Cases

- Happy path: `Application` can be constructed and returns ready in a headless environment.
- Edge cases: Build works even if SDL is not available yet.
- Error cases: Missing public header should fail compilation during the RED step.

## Test Execution

- Command(s): `c++ -std=c++20 -I. tests/smoke/test_startup.cpp -o /tmp/resonance_task1_smoke`
- Environment: macOS local shell, no SDL dependency required for the smoke test.
- Data fixtures: none.
- Expected duration: under 5 seconds.

## Risks

- Known flakiness: none.
- External dependencies: `cmake` is currently not available on `PATH`, so compilation is used for the initial TDD loop.
- Follow-up work: validate the generated `CMake` project once a `cmake` executable is available.
