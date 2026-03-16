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
    const bool has_nearby_anchor = std::find(before.begin(), before.end(), std::string{"Nearby Anchor: meadow-swing"}) != before.end();
    const bool has_visible_events_zero = std::find(before.begin(), before.end(), std::string{"Visible Events: distant_bell,passing_shadow"}) != before.end();
    const bool has_next_unlock_zero = std::find(before.begin(), before.end(), std::string{"Next Event Unlock: swing_memory_echo<-memory:meadow-swing"}) != before.end();

    scene.interact();
    scene.update();
    const auto after = scene.overlay_lines();
    const bool has_story = std::any_of(after.begin(), after.end(), [](const std::string& line) {
        return line.find("Story: ") == 0;
    });
    const bool has_active_anchor = std::find(after.begin(), after.end(), std::string{"Active Anchor: meadow-swing"}) != after.end();
    const bool has_visible_events_one = std::find(after.begin(), after.end(), std::string{"Visible Events: swing_memory_echo,distant_bell,passing_shadow"}) != after.end();
    const bool has_next_unlock_one = std::find(after.begin(), after.end(), std::string{"Next Event Unlock: <none>"}) != after.end();

    return (has_prompt &&
            has_nearby_anchor &&
            has_visible_events_zero &&
            has_next_unlock_zero &&
            has_story &&
            has_active_anchor &&
            has_visible_events_one &&
            has_next_unlock_one &&
            scene.current_story_anchor_id() == "meadow-swing")
        ? 0
        : 1;
}
