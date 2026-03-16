#include "engine/event/EventData.h"
#include "game/demo/DemoScene.h"

int main() {
    const auto bundle = resonance::load_demo_content("assets/data");
    resonance::DemoScene scene(bundle);

    scene.set_player_position({96.0F, 80.0F});
    scene.update();
    scene.interact();
    scene.update();

    scene.set_player_position({420.0F, 320.0F});
    scene.update();
    const auto station_before = scene.current_event_id();

    scene.set_player_position({372.0F, 108.0F});
    scene.update();
    scene.interact();
    scene.update();

    scene.set_player_position({420.0F, 320.0F});
    scene.update();
    const auto station_after = scene.current_event_id();

    return (station_before == "echoing_announcement" && station_after == "platform_convergence") ? 0 : 1;
}
