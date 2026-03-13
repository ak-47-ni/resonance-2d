# Task 3 Content Loading Test Plan

## Summary

- Feature/Change: Add JSON-based demo content loading for regions, music states, and events.
- Module/Area: `engine/core/FileSystem`, `engine/world/RegionData`, `engine/audio/MusicStateData`, `engine/event/EventData`.
- Scope: Phase 1 data loading and offline validation only.

## Test Mapping (1:1)

| Code Change | File/Function | Test Change | Test File |
| --- | --- | --- | --- |
| Add file reading helper | `engine/core/FileSystem.*` | Load demo content from disk in one integration-style smoke test | `tests/data/test_content_load.cpp` |
| Add region/music/event data loaders | `engine/world/RegionData.*`, `engine/audio/MusicStateData.*`, `engine/event/EventData.*` | Verify all three collections are non-empty | `tests/data/test_content_load.cpp` |
| Add demo data files | `assets/data/**/*.json` | Verify real files parse and cross-reference cleanly | `tests/data/test_content_load.cpp`, `tools/validate_assets.py` |

## Test Cases

- Happy path: valid demo JSON files load with non-empty collections.
- Edge cases: missing optional arrays default to empty collections.
- Error cases: missing files or malformed JSON throw clear errors.

## Test Execution

- Command(s): `cmake --build build && ctest --test-dir build -R content --output-on-failure && python3 tools/validate_assets.py`
- Environment: local macOS shell with `cmake`, `ninja`, and `nlohmann-json` installed.
- Data fixtures: files under `assets/data/`.
- Expected duration: under 10 seconds.

## Risks

- Known flakiness: none.
- External dependencies: JSON dependency should resolve through Homebrew package config.
- Follow-up work: add schema-level assertions as data complexity grows.
