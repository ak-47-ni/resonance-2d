# Resonance 2D MVP Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Build the first playable `resonance-2d` prototype with a C++ runtime, a small demo world, region-aware audio behavior, weighted narrative events, and lightweight debug tools.

**Architecture:** The runtime is split into focused engine modules: `core`, `render`, `world`, `audio`, `event`, and `debug`. A small demo game binds the modules together through JSON content files. `Python` tools validate content and catch broken references before runtime.

**Tech Stack:** `C++20`, `CMake`, `SDL3`, `SDL3_image`, `SDL3_mixer`, `CTest`, `Python 3`, `JSON`

---

### Task 1: Scaffold the project and build system

**Files:**
- Create: `CMakeLists.txt`
- Create: `cmake/Warnings.cmake`
- Create: `engine/core/Application.h`
- Create: `engine/core/Application.cpp`
- Create: `game/demo/main.cpp`
- Create: `tests/smoke/test_startup.cpp`
- Create: `assets/.gitkeep`
- Create: `tools/.gitkeep`

**Step 1: Write the failing test**

Create a startup smoke test that expects an `Application` type and a callable bootstrap path:

```cpp
#include "engine/core/Application.h"

int main() {
    resonance::Application app;
    return app.is_headless_ready() ? 0 : 1;
}
```

**Step 2: Run test to verify it fails**

Run: `cmake -S . -B build && cmake --build build && ctest --test-dir build --output-on-failure`

Expected: FAIL because the headers and implementation do not exist yet.

**Step 3: Write minimal implementation**

Add a minimal `Application` class with a cheap headless-ready check, and wire it into CMake so the test and demo executable build.

```cpp
namespace resonance {
class Application {
public:
    bool is_headless_ready() const { return true; }
};
}
```

**Step 4: Run test to verify it passes**

Run: `cmake --build build && ctest --test-dir build --output-on-failure`

Expected: PASS with one smoke test.

**Step 5: Commit**

```bash
git add CMakeLists.txt cmake/Warnings.cmake engine/core/Application.h engine/core/Application.cpp game/demo/main.cpp tests/smoke/test_startup.cpp assets/.gitkeep tools/.gitkeep
git commit -m "feat: bootstrap resonance-2d project"
```

### Task 2: Add SDL window loop and timing core

**Files:**
- Modify: `engine/core/Application.h`
- Modify: `engine/core/Application.cpp`
- Create: `engine/core/Time.h`
- Create: `engine/core/Time.cpp`
- Create: `tests/core/test_time.cpp`

**Step 1: Write the failing test**

Add a time-step test that verifies delta values are non-negative and frame accumulation is monotonic.

```cpp
int main() {
    resonance::Time time;
    time.tick(0.016);
    time.tick(0.020);
    return (time.delta_seconds() > 0.0 && time.total_seconds() > 0.03) ? 0 : 1;
}
```

**Step 2: Run test to verify it fails**

Run: `cmake --build build && ctest --test-dir build -R time --output-on-failure`

Expected: FAIL because `Time` does not exist.

**Step 3: Write minimal implementation**

Implement `Time` and extend `Application` to own an SDL event loop that can start, pump events, and stop cleanly.

```cpp
class Time {
public:
    void tick(double delta);
    double delta_seconds() const;
    double total_seconds() const;
};
```

**Step 4: Run test to verify it passes**

Run: `cmake --build build && ctest --test-dir build -R "startup|time" --output-on-failure`

Expected: PASS for startup and time tests.

**Step 5: Commit**

```bash
git add engine/core/Application.h engine/core/Application.cpp engine/core/Time.h engine/core/Time.cpp tests/core/test_time.cpp
git commit -m "feat: add runtime timing core"
```

### Task 3: Add content loading and data validation

**Files:**
- Create: `engine/core/FileSystem.h`
- Create: `engine/core/FileSystem.cpp`
- Create: `engine/world/RegionData.h`
- Create: `engine/world/RegionData.cpp`
- Create: `engine/audio/MusicStateData.h`
- Create: `engine/audio/MusicStateData.cpp`
- Create: `engine/event/EventData.h`
- Create: `engine/event/EventData.cpp`
- Create: `assets/data/regions/regions.json`
- Create: `assets/data/music/music_states.json`
- Create: `assets/data/events/events.json`
- Create: `tests/data/test_content_load.cpp`
- Create: `tools/validate_assets.py`

**Step 1: Write the failing test**

Add a test that loads all three JSON files and expects non-empty region, music, and event collections.

```cpp
int main() {
    auto bundle = resonance::load_demo_content("assets/data");
    return (!bundle.regions.empty() && !bundle.music_states.empty() && !bundle.events.empty()) ? 0 : 1;
}
```

**Step 2: Run test to verify it fails**

Run: `cmake --build build && ctest --test-dir build -R content --output-on-failure`

Expected: FAIL because loaders and files do not exist.

**Step 3: Write minimal implementation**

Create simple JSON schemas and loaders, then add a Python validator that checks file presence and cross-references.

```python
def main() -> int:
    # load JSON, verify referenced music states and regions exist
    return 0
```

**Step 4: Run test to verify it passes**

Run: `cmake --build build && ctest --test-dir build -R content --output-on-failure && python3 tools/validate_assets.py`

Expected: PASS and validator exits `0`.

**Step 5: Commit**

```bash
git add engine/core/FileSystem.h engine/core/FileSystem.cpp engine/world/RegionData.h engine/world/RegionData.cpp engine/audio/MusicStateData.h engine/audio/MusicStateData.cpp engine/event/EventData.h engine/event/EventData.cpp assets/data/regions/regions.json assets/data/music/music_states.json assets/data/events/events.json tests/data/test_content_load.cpp tools/validate_assets.py
git commit -m "feat: add demo content loading and validation"
```

### Task 4: Implement world regions and player traversal

**Files:**
- Create: `engine/world/World.h`
- Create: `engine/world/World.cpp`
- Create: `engine/world/Trigger.h`
- Create: `engine/world/Trigger.cpp`
- Create: `game/demo/DemoScene.h`
- Create: `game/demo/DemoScene.cpp`
- Create: `tests/world/test_region_switch.cpp`

**Step 1: Write the failing test**

Add a region-switch test that moves a player position between two rectangular regions and expects the active region id to update.

```cpp
int main() {
    resonance::World world = resonance::World::from_demo_data();
    world.set_player_position({10, 10});
    auto first = world.current_region_id();
    world.set_player_position({400, 10});
    auto second = world.current_region_id();
    return (first != second) ? 0 : 1;
}
```

**Step 2: Run test to verify it fails**

Run: `cmake --build build && ctest --test-dir build -R region_switch --output-on-failure`

Expected: FAIL because `World` logic does not exist.

**Step 3: Write minimal implementation**

Implement a rectangular-region world model and hook the demo scene to move a placeholder player entity.

```cpp
struct RectRegion {
    std::string id;
    float x;
    float y;
    float w;
    float h;
};
```

**Step 4: Run test to verify it passes**

Run: `cmake --build build && ctest --test-dir build -R "region_switch|content" --output-on-failure`

Expected: PASS and the demo scene launches with a movable placeholder.

**Step 5: Commit**

```bash
git add engine/world/World.h engine/world/World.cpp engine/world/Trigger.h engine/world/Trigger.cpp game/demo/DemoScene.h game/demo/DemoScene.cpp tests/world/test_region_switch.cpp
git commit -m "feat: add region-aware world traversal"
```

### Task 5: Implement `AudioDirector` for music and ambience

**Files:**
- Create: `engine/audio/AudioDirector.h`
- Create: `engine/audio/AudioDirector.cpp`
- Create: `engine/audio/AudioBackend.h`
- Create: `engine/audio/AudioBackendSDL.cpp`
- Create: `tests/audio/test_audio_state.cpp`

**Step 1: Write the failing test**

Add a test that feeds region changes and cue requests into `AudioDirector` and expects a deterministic resolved music state.

```cpp
int main() {
    resonance::AudioDirector audio;
    audio.set_region_music("forest", "explore");
    audio.enter_region("forest");
    audio.request_story_cue("mysterious");
    return (audio.current_music_state() == "mysterious") ? 0 : 1;
}
```

**Step 2: Run test to verify it fails**

Run: `cmake --build build && ctest --test-dir build -R audio_state --output-on-failure`

Expected: FAIL because `AudioDirector` does not exist.

**Step 3: Write minimal implementation**

Create an `AudioDirector` that resolves music state separately from low-level playback. The SDL mixer backend can begin as a thin wrapper.

```cpp
class AudioDirector {
public:
    void enter_region(const std::string& region_id);
    void request_story_cue(const std::string& state_id);
    std::string current_music_state() const;
};
```

**Step 4: Run test to verify it passes**

Run: `cmake --build build && ctest --test-dir build -R audio_state --output-on-failure`

Expected: PASS with deterministic state resolution.

**Step 5: Commit**

```bash
git add engine/audio/AudioDirector.h engine/audio/AudioDirector.cpp engine/audio/AudioBackend.h engine/audio/AudioBackendSDL.cpp tests/audio/test_audio_state.cpp
git commit -m "feat: add audio director with SDL backend"
```

### Task 6: Implement `EventDirector` with weighted narrative events

**Files:**
- Create: `engine/event/EventDirector.h`
- Create: `engine/event/EventDirector.cpp`
- Create: `engine/event/EventContext.h`
- Create: `tests/event/test_event_director.cpp`

**Step 1: Write the failing test**

Add a test that supplies a world context and expects only eligible events to be considered, with cooldowns respected.

```cpp
int main() {
    resonance::EventDirector director;
    auto context = resonance::EventContext::demo();
    auto result = director.pick_event(context);
    return (result.has_value() && result->id == "old_broadcast_echo") ? 0 : 1;
}
```

**Step 2: Run test to verify it fails**

Run: `cmake --build build && ctest --test-dir build -R event_director --output-on-failure`

Expected: FAIL because the selector does not exist.

**Step 3: Write minimal implementation**

Implement filtering by region, tags, cooldown, and weighted selection. Keep RNG injectable so tests can stay deterministic.

```cpp
struct EventContext {
    std::string region_id;
    std::unordered_set<std::string> world_tags;
    double seconds_since_last_major_event;
};
```

**Step 4: Run test to verify it passes**

Run: `cmake --build build && ctest --test-dir build -R event_director --output-on-failure`

Expected: PASS with deterministic event choice under a fixed RNG seed.

**Step 5: Commit**

```bash
git add engine/event/EventDirector.h engine/event/EventDirector.cpp engine/event/EventContext.h tests/event/test_event_director.cpp
git commit -m "feat: add weighted event director"
```

### Task 7: Add debug overlay and decision tracing

**Files:**
- Create: `engine/debug/DebugOverlay.h`
- Create: `engine/debug/DebugOverlay.cpp`
- Create: `engine/debug/TraceLog.h`
- Create: `engine/debug/TraceLog.cpp`
- Create: `tests/debug/test_trace_log.cpp`

**Step 1: Write the failing test**

Add a test that appends trace entries and expects only the configured rolling window to remain.

```cpp
int main() {
    resonance::TraceLog log(3);
    log.push("a");
    log.push("b");
    log.push("c");
    log.push("d");
    return (log.entries().front() == "b" && log.entries().size() == 3) ? 0 : 1;
}
```

**Step 2: Run test to verify it fails**

Run: `cmake --build build && ctest --test-dir build -R trace_log --output-on-failure`

Expected: FAIL because the debug classes do not exist.

**Step 3: Write minimal implementation**

Implement a small trace log and render it in an overlay panel that shows region, music state, and recent events.

```cpp
class TraceLog {
public:
    explicit TraceLog(std::size_t max_entries);
    void push(std::string entry);
};
```

**Step 4: Run test to verify it passes**

Run: `cmake --build build && ctest --test-dir build -R trace_log --output-on-failure`

Expected: PASS and the demo shows overlay text in development mode.

**Step 5: Commit**

```bash
git add engine/debug/DebugOverlay.h engine/debug/DebugOverlay.cpp engine/debug/TraceLog.h engine/debug/TraceLog.cpp tests/debug/test_trace_log.cpp
git commit -m "feat: add debug overlay and trace log"
```

### Task 8: Wire the demo scene together and verify the experience

**Files:**
- Modify: `game/demo/main.cpp`
- Modify: `game/demo/DemoScene.cpp`
- Modify: `assets/data/regions/regions.json`
- Modify: `assets/data/music/music_states.json`
- Modify: `assets/data/events/events.json`
- Create: `docs/demo-walkthrough.md`

**Step 1: Write the failing test**

Add a smoke test or manual verification checklist that expects the demo to expose at least three regions and one eligible event in the starting area.

```cpp
int main() {
    auto bundle = resonance::load_demo_content("assets/data");
    return (bundle.regions.size() >= 3 && bundle.events.size() >= 5) ? 0 : 1;
}
```

**Step 2: Run test to verify it fails**

Run: `cmake --build build && ctest --test-dir build --output-on-failure`

Expected: FAIL until demo content reaches the MVP thresholds.

**Step 3: Write minimal implementation**

Populate the demo content with three to five regions, at least five events, and three music states. Document how to walk through the demo and what to observe.

```markdown
1. Start in the meadow.
2. Walk to the ruins.
3. Watch the debug overlay report the region and music shift.
```

**Step 4: Run test to verify it passes**

Run: `cmake --build build && ctest --test-dir build --output-on-failure && python3 tools/validate_assets.py`

Expected: PASS and the demo walkthrough can be followed end-to-end.

**Step 5: Commit**

```bash
git add game/demo/main.cpp game/demo/DemoScene.cpp assets/data/regions/regions.json assets/data/music/music_states.json assets/data/events/events.json docs/demo-walkthrough.md
git commit -m "feat: ship first resonance-2d demo slice"
```

### Task 9: Final verification before completion

**Files:**
- Modify: `docs/demo-walkthrough.md`
- Create: `docs/verification-checklist.md`

**Step 1: Write the verification checklist**

Document exact commands and the expected manual observations for the MVP.

```markdown
- Build succeeds
- Tests pass
- Validator exits 0
- Region changes affect audio state
- Debug overlay explains the active event and music state
```

**Step 2: Run automated verification**

Run: `cmake -S . -B build && cmake --build build && ctest --test-dir build --output-on-failure && python3 tools/validate_assets.py`

Expected: All commands succeed.

**Step 3: Run manual verification**

Run the demo executable and follow `docs/demo-walkthrough.md`.

Expected: Mood changes are visible and audible when moving across regions.

**Step 4: Update docs with results**

Record what worked, what felt weak, and the next improvements to pursue.

```markdown
## Findings
- Crossfade timing feels abrupt at 200 ms.
- Event frequency feels good in the ruins region.
```

**Step 5: Commit**

```bash
git add docs/demo-walkthrough.md docs/verification-checklist.md
git commit -m "docs: record resonance-2d MVP verification"
```

## Execution Notes

- Prefer `@test-driven-development` discipline while implementing each task.
- Use `@verification-before-completion` before claiming the MVP works.
- Keep the runtime simple; avoid adding scripting, physics, or editor features during the MVP.
- If dependency setup becomes painful, preserve interfaces first and stub implementations behind them.
