#include "engine/event/EventData.h"
#include "game/demo/DemoScene.h"

#include <cmath>

int main() {
    const auto bundle = resonance::load_demo_content("assets/data");
    resonance::DemoScene scene(bundle);

    scene.set_player_position({96.0F, 80.0F});
    scene.update();
    const float center_focus = scene.current_story_focus();

    scene.set_player_position({110.0F, 80.0F});
    scene.update();
    const float halfway_focus = scene.current_story_focus();

    scene.set_player_position({140.0F, 80.0F});
    scene.update();
    const float far_focus = scene.current_story_focus();

    const auto approx_equal = [](float lhs, float rhs) {
        return std::fabs(lhs - rhs) < 0.0001F;
    };

    return (approx_equal(center_focus, 1.0F) &&
            approx_equal(halfway_focus, 0.5F) &&
            approx_equal(far_focus, 0.0F))
        ? 0
        : 1;
}
