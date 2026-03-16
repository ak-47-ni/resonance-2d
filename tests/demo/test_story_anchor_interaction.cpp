#include "engine/event/EventData.h"
#include "game/demo/DemoScene.h"

#include <algorithm>
#include <string>

int main() {
    const auto bundle = resonance::load_demo_content("assets/data");
    resonance::DemoScene scene(bundle);

    scene.set_player_position({96.0F, 80.0F});
    scene.update();
    const auto before = scene.overlay_lines();
    const bool has_prompt = std::find(before.begin(), before.end(), std::string{"Action: Press E to listen"}) != before.end();

    scene.interact();
    scene.update();
    const auto after = scene.overlay_lines();
    const bool has_story = std::any_of(after.begin(), after.end(), [](const std::string& line) {
        return line.find("Story: ") == 0;
    });

    return (has_prompt && has_story && scene.current_story_anchor_id() == "meadow-swing") ? 0 : 1;
}
