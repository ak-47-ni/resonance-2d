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
    const bool has_region = std::find(lines.begin(), lines.end(), std::string{"Region: meadow"}) != lines.end();
    const bool has_story = std::any_of(lines.begin(), lines.end(), [](const std::string& line) {
        return line.find("Memory: ") == 0;
    });
    const bool has_chain_stage = std::find(lines.begin(), lines.end(), std::string{"Memory Chain: 1/3"}) != lines.end();
    const bool has_station_chain = std::find(lines.begin(), lines.end(), std::string{"Station Chain: echoing_announcement"}) != lines.end();
    const bool has_next_memory = std::find(lines.begin(), lines.end(), std::string{"Next Memory: ruins-gate"}) != lines.end();
    const bool has_next_station_upgrade = std::find(lines.begin(), lines.end(), std::string{"Next Station Upgrade: platform_convergence"}) != lines.end();
    const bool has_visible_events = std::find(lines.begin(), lines.end(), std::string{"Visible Events: swing_memory_echo,distant_bell,passing_shadow"}) != lines.end();
    const bool has_next_unlock = std::find(lines.begin(), lines.end(), std::string{"Next Event Unlock: <none>"}) != lines.end();

    return (has_title &&
            has_entry &&
            has_region &&
            has_story &&
            has_chain_stage &&
            has_station_chain &&
            has_next_memory &&
            has_next_station_upgrade &&
            has_visible_events &&
            has_next_unlock)
        ? 0
        : 1;
}
