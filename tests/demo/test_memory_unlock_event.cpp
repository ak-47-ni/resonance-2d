#include "engine/event/EventData.h"
#include "game/demo/DemoScene.h"

#include <string>

int main() {
    const auto bundle = resonance::load_demo_content("assets/data");
    resonance::DemoScene scene(bundle);

    scene.set_player_position({96.0F, 80.0F});
    scene.update();
    const auto initial = scene.current_event_id();

    scene.interact();
    scene.update();

    for (int index = 0; index < 8; ++index) {
        scene.update();
    }

    return (initial == "distant_bell" && scene.current_event_id() == "swing_memory_echo") ? 0 : 1;
}
