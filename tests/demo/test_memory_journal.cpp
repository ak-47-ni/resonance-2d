#include "engine/event/EventData.h"
#include "game/demo/DemoScene.h"

int main() {
    const auto bundle = resonance::load_demo_content("assets/data");
    resonance::DemoScene scene(bundle);

    scene.set_player_position({96.0F, 80.0F});
    scene.update();
    scene.interact();
    scene.update();
    const auto first = scene.memory_journal_entries();

    scene.interact();
    scene.update();
    const auto second = scene.memory_journal_entries();

    return (first.size() == 1U &&
            second.size() == 1U &&
            first.front().id == "meadow-swing" &&
            first.front().region_id == "meadow" &&
            !first.front().story_text.empty())
        ? 0
        : 1;
}
