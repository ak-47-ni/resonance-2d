# Task 6 Event Director Test Plan

## Summary

- Feature/Change: Add an `EventDirector` that selects an eligible narrative event from the loaded event table using region context and weight.
- Module/Area: `engine/event/EventDirector`, `engine/event/EventContext`.
- Scope: Deterministic phase-1 event selection only.

## Test Mapping (1:1)

| Code Change | File/Function | Test Change | Test File |
| --- | --- | --- | --- |
| Add event selection context | `engine/event/EventContext.h` | Construct a demo context for the selector | `tests/event/test_event_director.cpp` |
| Add event selector | `engine/event/EventDirector.*` | Verify the highest-weight eligible event is chosen for the current region | `tests/event/test_event_director.cpp` |

## Test Cases

- Happy path: for `ruins`, the selector returns `old_broadcast_echo` because it has the strongest weight.
- Edge cases: when no event matches the region, the selector returns no result.
- Error cases: empty event lists do not crash the selector.

## Test Execution

- Command(s): `cmake --build build && ctest --test-dir build -R event_director --output-on-failure`
- Environment: local macOS shell with current project dependencies.
- Data fixtures: `assets/data/events/events.json`
- Expected duration: under 10 seconds.

## Risks

- Known flakiness: none.
- External dependencies: none.
- Follow-up work: extend eligibility with tags and cooldowns in later tasks.
