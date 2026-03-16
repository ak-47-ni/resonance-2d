#include "engine/audio/AudioToneProfile.h"

namespace {

resonance::AudioStateSnapshot make_snapshot(const char* music_state, std::vector<std::string> ambient_layers) {
    resonance::AudioStateSnapshot snapshot;
    snapshot.music_state = music_state;
    snapshot.fallback_music_state = music_state;
    snapshot.ambient_layers = std::move(ambient_layers);
    return snapshot;
}

}  // namespace

int main() {
    const auto explore = resonance::make_tone_profile(make_snapshot("explore", {"wind"}));
    const auto mysterious = resonance::make_tone_profile(make_snapshot("mysterious", {"rumble"}));
    const auto silent = resonance::make_tone_profile(make_snapshot("", {}));
    const auto fallback = resonance::make_tone_profile(make_snapshot("unknown", {}));

    return (explore.master_gain > 0.0F &&
            mysterious.master_gain > 0.0F &&
            mysterious.carrier_frequency != explore.carrier_frequency &&
            silent.master_gain == 0.0F &&
            fallback.master_gain > 0.0F)
        ? 0
        : 1;
}
