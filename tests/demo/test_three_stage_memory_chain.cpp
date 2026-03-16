#include "engine/event/EventData.h"
#include "game/demo/DemoScene.h"

int main() {
    const auto bundle = resonance::load_demo_content("assets/data");
    resonance::DemoScene scene(bundle);

    scene.set_player_position({96.0F, 80.0F});
    scene.update();
    scene.interact();
    scene.update();

    scene.set_player_position({372.0F, 108.0F});
    scene.update();
    scene.interact();
    scene.update();

    scene.set_player_position({420.0F, 320.0F});
    scene.update();
    const auto stage_two = scene.current_event_id();

    scene.set_player_position({112.0F, 392.0F});
    scene.update();
    scene.interact();
    scene.update();

    scene.set_player_position({420.0F, 320.0F});
    scene.update();
    const auto stage_three = scene.current_event_id();

    return (stage_two == "platform_convergence" && stage_three == "terminal_refrain") ? 0 : 1;
}
