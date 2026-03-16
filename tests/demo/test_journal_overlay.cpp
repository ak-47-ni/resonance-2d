#include "engine/event/EventData.h"
#include "game/demo/DemoScene.h"

#include <algorithm>
#include <string>

int main() {
    const auto bundle = resonance::load_demo_content("assets/data");
    resonance::DemoScene scene(bundle);

    scene.set_player_position({96.0F, 80.0F});
    scene.update();
    scene.interact();
    scene.toggle_journal();
    scene.update();

    const auto lines = scene.overlay_lines();
    const bool has_title = std::find(lines.begin(), lines.end(), std::string{"Journal"}) != lines.end();
    const bool has_entry = std::any_of(lines.begin(), lines.end(), [](const std::string& line) {
        return line.find("Entry: meadow-swing") != std::string::npos;
    });
    const bool has_story = std::any_of(lines.begin(), lines.end(), [](const std::string& line) {
        return line.find("Memory: ") == 0;
    });

    return (has_title && has_entry && has_story) ? 0 : 1;
}
