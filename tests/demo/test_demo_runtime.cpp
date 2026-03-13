#include "engine/event/EventData.h"
#include "game/demo/DemoScene.h"

#include <algorithm>
#include <string>

int main() {
    const auto bundle = resonance::load_demo_content("assets/data");
    resonance::DemoScene scene(bundle);

    scene.set_player_position({10.0F, 10.0F});
    scene.update();

    const auto lines = scene.overlay_lines();
    const bool has_region_line = std::find(lines.begin(), lines.end(), std::string{"Region: meadow"}) != lines.end();
    const bool has_music_line = std::find(lines.begin(), lines.end(), std::string{"Music: mysterious"}) != lines.end();

    scene.set_player_position({999.0F, 999.0F});
    scene.update();

    return (scene.current_region_id() == "" &&
            scene.current_music_state() == "" &&
            scene.current_event_id() == "" &&
            has_region_line &&
            has_music_line)
        ? 0
        : 1;
}
