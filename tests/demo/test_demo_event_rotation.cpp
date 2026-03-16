#include "engine/event/EventData.h"
#include "game/demo/DemoScene.h"

#include <string>

int main() {
    const auto bundle = resonance::load_demo_content("assets/data");
    resonance::DemoScene scene(bundle);

    scene.set_player_position({10.0F, 10.0F});
    scene.update();
    const auto first = scene.current_event_id();

    scene.update();
    const auto immediate = scene.current_event_id();

    for (int index = 0; index < 8; ++index) {
        scene.update();
    }
    const auto rotated = scene.current_event_id();

    return (first == "distant_bell" &&
            immediate == "distant_bell" &&
            rotated == "passing_shadow")
        ? 0
        : 1;
}
