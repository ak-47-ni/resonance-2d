#include "engine/event/EventData.h"
#include "game/demo/DemoScene.h"

#include <cmath>

int main() {
    const auto bundle = resonance::load_demo_content("assets/data");
    resonance::DemoScene scene(bundle);

    scene.set_player_position({10.0F, 10.0F});
    scene.update();
    const auto first_event = scene.current_event_id();
    const float initial_emphasis = scene.current_event_emphasis();
    const float initial_bgm_gain = scene.current_bgm_gain();
    const float initial_ambient_gain = scene.current_ambient_gain_multiplier();

    scene.update();
    const float decayed_emphasis = scene.current_event_emphasis();
    const float decayed_bgm_gain = scene.current_bgm_gain();
    const float decayed_ambient_gain = scene.current_ambient_gain_multiplier();

    for (int index = 0; index < 4; ++index) {
        scene.update();
    }
    const float faded_emphasis = scene.current_event_emphasis();
    const float faded_bgm_gain = scene.current_bgm_gain();
    const float faded_ambient_gain = scene.current_ambient_gain_multiplier();

    const auto approx_equal = [](float lhs, float rhs) {
        return std::fabs(lhs - rhs) < 0.0001F;
    };

    return (first_event == "distant_bell" &&
            approx_equal(initial_emphasis, 1.0F) &&
            initial_bgm_gain < decayed_bgm_gain &&
            decayed_bgm_gain < 1.0F &&
            initial_ambient_gain > decayed_ambient_gain &&
            decayed_ambient_gain > 1.0F &&
            decayed_emphasis < initial_emphasis &&
            decayed_emphasis > 0.0F &&
            approx_equal(faded_emphasis, 0.0F) &&
            approx_equal(faded_bgm_gain, 1.0F) &&
            approx_equal(faded_ambient_gain, 1.0F))
        ? 0
        : 1;
}
