#include "engine/event/EventData.h"
#include "engine/world/World.h"

int main() {
    const auto bundle = resonance::load_demo_content("assets/data");
    resonance::World world(bundle.regions);

    world.set_player_position({10.0F, 10.0F});
    const auto first = world.current_region_id();

    world.set_player_position({420.0F, 10.0F});
    const auto second = world.current_region_id();

    world.set_player_position({999.0F, 999.0F});
    const auto outside = world.current_region_id();

    return (first == "meadow" && second == "ruins" && outside.empty()) ? 0 : 1;
}
