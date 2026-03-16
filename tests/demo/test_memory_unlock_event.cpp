#include "engine/event/EventData.h"
#include "game/demo/DemoScene.h"

#include <string>

int main() {
    const auto bundle = resonance::load_demo_content("assets/data");
    resonance::DemoScene scene(bundle);

    scene.set_player_position({96.0F, 80.0F});
    scene.update();
    const auto initial = scene.current_event_id();
    const auto initial_summary = scene.debug_summary();

    scene.interact();
    scene.update();

    for (int index = 0; index < 8; ++index) {
        scene.update();
    }

    const auto unlocked_summary = scene.debug_summary();
    const bool initial_hints = initial_summary.find("VisibleEvents=distant_bell,passing_shadow") != std::string::npos &&
        initial_summary.find("NextEventUnlock=swing_memory_echo<-memory:meadow-swing") != std::string::npos;
    const bool unlocked_hints = unlocked_summary.find("VisibleEvents=swing_memory_echo,distant_bell,passing_shadow") != std::string::npos &&
        unlocked_summary.find("NextEventUnlock=<none>") != std::string::npos;

    return (initial == "distant_bell" &&
            scene.current_event_id() == "swing_memory_echo" &&
            initial_hints &&
            unlocked_hints)
        ? 0
        : 1;
}
