#pragma once

#include "engine/audio/AudioBackend.h"

namespace resonance {

struct AudioToneProfile {
    float carrier_frequency = 0.0F;
    float layer_frequency = 0.0F;
    float master_gain = 0.0F;
    float layer_gain = 0.0F;
};

AudioToneProfile make_tone_profile(const AudioStateSnapshot& snapshot);

}  // namespace resonance
