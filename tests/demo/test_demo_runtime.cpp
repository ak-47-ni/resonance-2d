#include "engine/event/EventData.h"
#include "game/demo/DemoScene.h"

#include <algorithm>
#include <string>

int main() {
    const auto bundle = resonance::load_demo_content("assets/data");
    resonance::DemoScene scene(bundle);

    scene.set_player_position({96.0F, 80.0F});
    scene.update();

    const auto lines = scene.overlay_lines();
    const bool has_region_line = std::find(lines.begin(), lines.end(), std::string{"Region: meadow"}) != lines.end();
    const bool has_music_line = std::find(lines.begin(), lines.end(), std::string{"Music: mysterious"}) != lines.end();
    const bool has_prompt_line = std::find(lines.begin(), lines.end(), std::string{"Action: Press E to listen"}) != lines.end();

    scene.interact();
    scene.update();

    const auto story_lines = scene.overlay_lines();
    const bool has_story_line = std::any_of(story_lines.begin(), story_lines.end(), [](const std::string& line) {
        return line.find("Story: ") == 0;
    });

    scene.set_player_position({999.0F, 999.0F});
    scene.update();

    return (scene.current_region_id() == "" &&
            scene.current_music_state() == "" &&
            scene.current_event_id() == "" &&
            scene.current_interaction_prompt() == "" &&
            has_region_line &&
            has_music_line &&
            has_prompt_line &&
            has_story_line)
        ? 0
        : 1;
}
