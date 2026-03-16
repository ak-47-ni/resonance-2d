#include "engine/event/EventData.h"
#include "game/demo/DemoScene.h"

#include <string>

int main() {
    const auto bundle = resonance::load_demo_content("assets/data");
    resonance::DemoScene scene(bundle);

    scene.set_player_position({96.0F, 80.0F});
    scene.update();

    const auto player = scene.player_position();
    const auto visuals = scene.story_anchor_visuals();
    const auto summary = scene.debug_summary();

    scene.interact();
    scene.update();
    const auto story_summary = scene.debug_summary();

    scene.set_player_position({999.0F, 999.0F});
    scene.update();
    const auto cleared = scene.debug_summary();

    const bool has_region = summary.find("Region=meadow") != std::string::npos;
    const bool has_music = summary.find("Music=mysterious") != std::string::npos;
    const bool has_event = summary.find("Event=distant_bell") != std::string::npos;
    const bool has_story_anchor = story_summary.find("StoryAnchor=meadow-swing") != std::string::npos;
    const bool cleared_event = cleared.find("Event=<none>") != std::string::npos;

    return (scene.region_count() == 4U &&
            visuals.size() == 4U &&
            player.x == 96.0F &&
            player.y == 80.0F &&
            has_region &&
            has_music &&
            has_event &&
            has_story_anchor &&
            cleared_event)
        ? 0
        : 1;
}
