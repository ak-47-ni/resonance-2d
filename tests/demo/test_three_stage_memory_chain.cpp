#include "engine/event/EventData.h"
#include "game/demo/DemoScene.h"

#include <string>

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
    const auto stage_two_summary = scene.debug_summary();

    scene.set_player_position({112.0F, 392.0F});
    scene.update();
    scene.interact();
    scene.update();

    scene.set_player_position({420.0F, 320.0F});
    scene.update();
    const auto stage_three = scene.current_event_id();
    const auto stage_three_summary = scene.debug_summary();

    const bool two_chain = stage_two_summary.find("VisibleEvents=platform_convergence,echoing_announcement") != std::string::npos &&
        stage_two_summary.find("NextEventUnlock=terminal_refrain<-memory:lakeside-reeds") != std::string::npos;
    const bool three_chain = stage_three_summary.find("VisibleEvents=terminal_refrain,platform_convergence,echoing_announcement") != std::string::npos &&
        stage_three_summary.find("NextEventUnlock=<none>") != std::string::npos;

    return (stage_two == "platform_convergence" &&
            stage_three == "terminal_refrain" &&
            two_chain &&
            three_chain)
        ? 0
        : 1;
}
