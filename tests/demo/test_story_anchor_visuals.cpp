#include "engine/event/EventData.h"
#include "game/demo/DemoScene.h"

#include <algorithm>
#include <string>

int main() {
    const auto bundle = resonance::load_demo_content("assets/data");
    resonance::DemoScene scene(bundle);

    scene.set_player_position({96.0F, 80.0F});
    scene.update();
    const auto before = scene.story_anchor_visuals();

    const auto nearby = std::find_if(before.begin(), before.end(), [](const resonance::StoryAnchorVisual& visual) {
        return visual.id == "meadow-swing" && visual.is_nearby && !visual.is_active;
    });

    scene.interact();
    scene.update();
    const auto after = scene.story_anchor_visuals();

    const auto active = std::find_if(after.begin(), after.end(), [](const resonance::StoryAnchorVisual& visual) {
        return visual.id == "meadow-swing" && visual.is_active;
    });

    return (before.size() == 4U && nearby != before.end() && active != after.end()) ? 0 : 1;
}
