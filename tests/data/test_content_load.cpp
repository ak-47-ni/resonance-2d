#include "engine/event/EventData.h"

#include <algorithm>
#include <cmath>
#include <string>

int main() {
    const auto bundle = resonance::load_demo_content("assets/data");

    auto approx_equal = [](float lhs, float rhs) {
        return std::fabs(lhs - rhs) < 0.0001F;
    };

    const auto distant_bell = std::find_if(bundle.events.begin(), bundle.events.end(), [](const resonance::EventData& event) {
        return event.id == "distant_bell";
    });
    const auto passing_shadow = std::find_if(bundle.events.begin(), bundle.events.end(), [](const resonance::EventData& event) {
        return event.id == "passing_shadow";
    });

    const bool content_ok = !bundle.regions.empty() &&
        !bundle.music_states.empty() &&
        !bundle.events.empty() &&
        !bundle.story_anchors.empty();

    const bool bell_ok = distant_bell != bundle.events.end() &&
        approx_equal(distant_bell->mix_profile.event_duck, 0.85F) &&
        approx_equal(distant_bell->mix_profile.ambient_boost, 0.65F);

    const bool shadow_ok = passing_shadow != bundle.events.end() &&
        approx_equal(passing_shadow->mix_profile.event_duck, 0.25F) &&
        approx_equal(passing_shadow->mix_profile.ambient_boost, 0.15F);

    return (content_ok && bell_ok && shadow_ok) ? 0 : 1;
}
