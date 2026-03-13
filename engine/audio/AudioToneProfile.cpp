#include "engine/audio/AudioToneProfile.h"

namespace resonance {

AudioToneProfile make_tone_profile(const AudioStateSnapshot& snapshot) {
    if (snapshot.music_state.empty()) {
        return {};
    }

    AudioToneProfile profile;
    if (snapshot.music_state == "explore") {
        profile.carrier_frequency = 220.0F;
        profile.layer_frequency = 330.0F;
        profile.master_gain = 0.08F;
        profile.layer_gain = 0.02F;
    } else if (snapshot.music_state == "mysterious") {
        profile.carrier_frequency = 164.81F;
        profile.layer_frequency = 246.94F;
        profile.master_gain = 0.10F;
        profile.layer_gain = 0.035F;
    } else if (snapshot.music_state == "calm") {
        profile.carrier_frequency = 196.0F;
        profile.layer_frequency = 293.66F;
        profile.master_gain = 0.07F;
        profile.layer_gain = 0.015F;
    } else {
        profile.carrier_frequency = 207.65F;
        profile.layer_frequency = 311.13F;
        profile.master_gain = 0.06F;
        profile.layer_gain = 0.01F;
    }

    if (snapshot.ambient_layers.empty()) {
        profile.layer_gain *= 0.5F;
    }

    return profile;
}

}  // namespace resonance
