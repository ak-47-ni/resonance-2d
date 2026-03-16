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

    scene.set_player_position({420.0F, 320.0F});
    scene.update();
    const auto station_before = scene.current_event_id();
    const auto station_before_summary = scene.debug_summary();

    scene.set_player_position({372.0F, 108.0F});
    scene.update();
    scene.interact();
    scene.update();

    scene.set_player_position({420.0F, 320.0F});
    scene.update();
    const auto station_after = scene.current_event_id();
    const auto station_after_summary = scene.debug_summary();

    const bool before_chain = station_before_summary.find("VisibleEvents=echoing_announcement") != std::string::npos &&
        station_before_summary.find("NextEventUnlock=platform_convergence<-memory:ruins-gate") != std::string::npos;
    const bool after_chain = station_after_summary.find("VisibleEvents=platform_convergence,echoing_announcement") != std::string::npos &&
        station_after_summary.find("NextEventUnlock=terminal_refrain<-memory:lakeside-reeds") != std::string::npos;

    return (station_before == "echoing_announcement" &&
            station_after == "platform_convergence" &&
            before_chain &&
            after_chain)
        ? 0
        : 1;
}
