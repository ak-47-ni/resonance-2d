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
    const bool has_nearby_anchor_line = std::find(lines.begin(), lines.end(), std::string{"Nearby Anchor: meadow-swing"}) != lines.end();
    const bool has_story_focus_line = std::find(lines.begin(), lines.end(), std::string{"Story Focus: 1.00"}) != lines.end();
    const bool has_event_emphasis_line = std::find(lines.begin(), lines.end(), std::string{"Event Emphasis: 1.00"}) != lines.end();
    const bool has_event_duck_line = std::find(lines.begin(), lines.end(), std::string{"Event Duck: 0.85"}) != lines.end();
    const bool has_ambient_boost_line = std::find(lines.begin(), lines.end(), std::string{"Ambient Boost: 0.65"}) != lines.end();
    const bool has_bgm_gain_line = std::find(lines.begin(), lines.end(), std::string{"Bgm Gain: 0.70"}) != lines.end();
    const bool has_ambient_gain_line = std::find(lines.begin(), lines.end(), std::string{"Ambient Gain: 1.86"}) != lines.end();

    scene.interact();
    scene.update();

    const auto story_lines = scene.overlay_lines();
    const bool has_story_line = std::any_of(story_lines.begin(), story_lines.end(), [](const std::string& line) {
        return line.find("Story: ") == 0;
    });
    const bool has_active_anchor_line = std::find(story_lines.begin(), story_lines.end(), std::string{"Active Anchor: meadow-swing"}) != story_lines.end();

    scene.set_player_position({999.0F, 999.0F});
    scene.update();

    return (scene.current_region_id() == "" &&
            scene.current_music_state() == "" &&
            scene.current_event_id() == "" &&
            scene.current_interaction_prompt() == "" &&
            has_region_line &&
            has_music_line &&
            has_prompt_line &&
            has_nearby_anchor_line &&
            has_story_focus_line &&
            has_event_emphasis_line &&
            has_event_duck_line &&
            has_ambient_boost_line &&
            has_bgm_gain_line &&
            has_ambient_gain_line &&
            has_story_line &&
            has_active_anchor_line)
        ? 0
        : 1;
}
