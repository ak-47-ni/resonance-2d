#pragma once

#include "game/demo/DemoScene.h"

#include <string>
#include <vector>

namespace resonance {

inline std::vector<std::string> run_memory_chain_debug_route(const DemoContentBundle& bundle) {
    DemoScene scene(bundle);
    std::vector<std::string> summaries;

    const auto capture = [&scene, &summaries](const std::string& label) {
        summaries.push_back("RouteStep=" + label + " | " + scene.debug_summary());
    };

    scene.set_player_position({10.0F, 10.0F});
    scene.update();
    capture("meadow-start");

    scene.set_player_position({96.0F, 80.0F});
    scene.update();
    scene.interact();
    scene.update();
    capture("meadow-memory");

    scene.set_player_position({372.0F, 108.0F});
    scene.update();
    scene.interact();
    scene.update();
    capture("ruins-memory");

    scene.set_player_position({420.0F, 320.0F});
    scene.update();
    capture("station-stage-two");

    scene.set_player_position({112.0F, 392.0F});
    scene.update();
    scene.interact();
    scene.update();
    scene.set_player_position({420.0F, 320.0F});
    scene.update();
    capture("station-stage-three");

    return summaries;
}

}  // namespace resonance
