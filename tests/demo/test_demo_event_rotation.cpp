#include "engine/event/EventData.h"
#include "game/demo/DemoScene.h"

#include <cmath>
#include <string>

int main() {
    const auto bundle = resonance::load_demo_content("assets/data");
    resonance::DemoScene scene(bundle);

    auto approx_equal = [](float lhs, float rhs) {
        return std::fabs(lhs - rhs) < 0.0001F;
    };

    scene.set_player_position({10.0F, 10.0F});
    scene.update();
    const auto first = scene.current_event_id();
    const auto first_profile = scene.current_event_mix_profile();
    const float first_bgm_gain = scene.current_bgm_gain();
    const float first_ambient_gain = scene.current_ambient_gain_multiplier();

    scene.update();
    const auto immediate = scene.current_event_id();
    const auto immediate_profile = scene.current_event_mix_profile();

    for (int index = 0; index < 8; ++index) {
        scene.update();
    }
    const auto rotated = scene.current_event_id();
    const auto rotated_profile = scene.current_event_mix_profile();
    const float rotated_bgm_gain = scene.current_bgm_gain();
    const float rotated_ambient_gain = scene.current_ambient_gain_multiplier();

    return (first == "distant_bell" &&
            immediate == "distant_bell" &&
            rotated == "passing_shadow" &&
            approx_equal(first_profile.event_duck, 0.85F) &&
            approx_equal(first_profile.ambient_boost, 0.65F) &&
            approx_equal(immediate_profile.event_duck, 0.85F) &&
            approx_equal(rotated_profile.event_duck, 0.25F) &&
            approx_equal(rotated_profile.ambient_boost, 0.15F) &&
            first_bgm_gain < rotated_bgm_gain &&
            first_ambient_gain > rotated_ambient_gain)
        ? 0
        : 1;
}
