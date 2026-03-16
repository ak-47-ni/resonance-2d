# Runtime Story Anchors Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Add lightweight proximity-based story anchors that the player can trigger in the demo runtime.

**Architecture:** Story anchors are loaded from a new JSON content file into `DemoContentBundle`, stored by `DemoScene`, and resolved by player proximity. The SDL runtime uses a small `interact()` hook and the debug overlay renders prompt and active story text.

**Tech Stack:** `C++20`, `SDL3`, `CMake`, `CTest`, `Python 3`, `nlohmann_json`

---

### Task 1: Add story-anchor content loading

**Files:**
- Create: `engine/world/StoryAnchorData.h`
- Create: `engine/world/StoryAnchorData.cpp`
- Modify: `engine/event/EventData.h`
- Modify: `engine/event/EventData.cpp`
- Create: `assets/data/story/story_anchors.json`
- Modify: `tools/validate_assets.py`
- Modify: `CMakeLists.txt`
- Create: `tests/data/test_story_anchor_content.cpp`

**Step 1: Write the failing test**

```cpp
#include "engine/event/EventData.h"

int main() {
    const auto bundle = resonance::load_demo_content("assets/data");
    return (!bundle.story_anchors.empty() && bundle.story_anchors.front().id == "meadow-swing") ? 0 : 1;
}
```

**Step 2: Run test to verify it fails**

Run: `cmake --build build && ctest --test-dir build -R story_anchor_content --output-on-failure`
Expected: FAIL because story-anchor content types and test target do not exist yet.

**Step 3: Write minimal implementation**

Create a loader for `story_anchors.json`, extend `DemoContentBundle`, wire the new file into `load_demo_content()`, and validate region references plus positive activation radii.

**Step 4: Run test to verify it passes**

Run: `cmake --build build && ctest --test-dir build -R story_anchor_content --output-on-failure && python3 tools/validate_assets.py`
Expected: PASS and validator exits `0`.

**Step 5: Commit**

```bash
git add engine/world/StoryAnchorData.h engine/world/StoryAnchorData.cpp engine/event/EventData.h engine/event/EventData.cpp assets/data/story/story_anchors.json tools/validate_assets.py CMakeLists.txt tests/data/test_story_anchor_content.cpp
git commit -m "feat(runtime): add story anchor content loading"
```

### Task 2: Add proximity resolution and interaction state

**Files:**
- Modify: `game/demo/DemoScene.h`
- Modify: `game/demo/DemoScene.cpp`
- Create: `tests/demo/test_story_anchor_interaction.cpp`
- Modify: `CMakeLists.txt`

**Step 1: Write the failing test**

```cpp
#include "engine/event/EventData.h"
#include "game/demo/DemoScene.h"

#include <algorithm>
#include <string>

int main() {
    const auto bundle = resonance::load_demo_content("assets/data");
    resonance::DemoScene scene(bundle);

    scene.set_player_position({96.0F, 80.0F});
    scene.update();
    const auto before = scene.overlay_lines();
    const bool has_prompt = std::find(before.begin(), before.end(), std::string{"Action: Press E to listen"}) != before.end();

    scene.interact();
    scene.update();
    const auto after = scene.overlay_lines();
    const bool has_story = std::any_of(after.begin(), after.end(), [](const std::string& line) {
        return line.find("Story: ") == 0;
    });

    return (has_prompt && has_story && scene.current_story_anchor_id() == "meadow-swing") ? 0 : 1;
}
```

**Step 2: Run test to verify it fails**

Run: `cmake --build build && ctest --test-dir build -R story_anchor_interaction --output-on-failure`
Expected: FAIL because interaction APIs do not exist yet.

**Step 3: Write minimal implementation**

Add proximity lookup, `interact()`, current anchor accessors, and active story state to `DemoScene`.

**Step 4: Run test to verify it passes**

Run: `cmake --build build && ctest --test-dir build -R story_anchor_interaction --output-on-failure`
Expected: PASS.

**Step 5: Commit**

```bash
git add game/demo/DemoScene.h game/demo/DemoScene.cpp tests/demo/test_story_anchor_interaction.cpp CMakeLists.txt
git commit -m "feat(runtime): add story anchor interaction state"
```

### Task 3: Surface the feature in overlay and SDL input

**Files:**
- Modify: `engine/debug/DebugOverlay.h`
- Modify: `engine/debug/DebugOverlay.cpp`
- Modify: `game/demo/main.cpp`
- Modify: `tests/debug/test_overlay_panel.cpp`
- Modify: `tests/demo/test_demo_runtime.cpp`
- Modify: `tests/demo/test_demo_visual_state.cpp`

**Step 1: Write the failing test**

```cpp
#include "engine/debug/DebugOverlay.h"
#include "engine/debug/TraceLog.h"

#include <algorithm>
#include <string>

int main() {
    resonance::DebugOverlay overlay;
    resonance::TraceLog log{4U};
    log.push("Event: distant_bell");

    const auto lines = overlay.build_lines("meadow", "mysterious", "distant_bell", "Press E to listen", "A rusted swing clicks in the wind.", log);
    const bool has_prompt = std::find(lines.begin(), lines.end(), std::string{"Action: Press E to listen"}) != lines.end();
    const bool has_story = std::find(lines.begin(), lines.end(), std::string{"Story: A rusted swing clicks in the wind."}) != lines.end();

    return (has_prompt && has_story) ? 0 : 1;
}
```

**Step 2: Run test to verify it fails**

Run: `cmake --build build && ctest --test-dir build -R 'overlay_panel|demo_runtime|demo_visual_state' --output-on-failure`
Expected: FAIL because overlay and runtime surfaces are not updated yet.

**Step 3: Write minimal implementation**

Extend overlay formatting and let `main.cpp` map `E` to `scene.interact()`.

**Step 4: Run test to verify it passes**

Run: `cmake --build build && ctest --test-dir build -R 'overlay_panel|demo_runtime|demo_visual_state|story_anchor_interaction' --output-on-failure`
Expected: PASS.

**Step 5: Commit**

```bash
git add engine/debug/DebugOverlay.h engine/debug/DebugOverlay.cpp game/demo/main.cpp tests/debug/test_overlay_panel.cpp tests/demo/test_demo_runtime.cpp tests/demo/test_demo_visual_state.cpp
git commit -m "feat(runtime): surface story anchor prompts in demo"
```

### Task 4: Run final verification

**Files:**
- Modify: `docs/demo-walkthrough.md`
- Create: `docs/plans/2026-03-13-task12-story-anchor-test-plan.md`

**Step 1: Update docs**

Document the new interaction flow and verification steps.

**Step 2: Run full verification**

Run: `cmake -S . -B build -G Ninja && cmake --build build && ctest --test-dir build --output-on-failure && python3 tools/validate_assets.py && SDL_AUDIODRIVER=dummy SDL_VIDEODRIVER=dummy RESONANCE_DEMO_HIDDEN=1 RESONANCE_DEMO_AUTOCLOSE=1 ./build/resonance_demo`
Expected: PASS.

**Step 3: Commit**

```bash
git add docs/demo-walkthrough.md docs/plans/2026-03-13-task12-story-anchor-test-plan.md
git commit -m "docs(runtime): document story anchor verification"
```
