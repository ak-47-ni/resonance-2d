#include "engine/audio/AudioToneProfile.h"

int main() {
    const auto explore = resonance::make_tone_profile({"explore", {"wind"}});
    const auto mysterious = resonance::make_tone_profile({"mysterious", {"rumble"}});
    const auto silent = resonance::make_tone_profile({"", {}});
    const auto fallback = resonance::make_tone_profile({"unknown", {}});

    return (explore.master_gain > 0.0F &&
            mysterious.master_gain > 0.0F &&
            mysterious.carrier_frequency != explore.carrier_frequency &&
            silent.master_gain == 0.0F &&
            fallback.master_gain > 0.0F)
        ? 0
        : 1;
}
