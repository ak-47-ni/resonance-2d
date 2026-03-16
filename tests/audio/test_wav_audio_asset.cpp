#include "engine/audio/WavAudioAsset.h"

#include <array>
#include <cmath>
#include <cstddef>
#include <vector>

namespace {

bool nearly_equal(float left, float right) {
    return std::fabs(left - right) < 0.0001F;
}

}  // namespace

int main() {
    const auto explore = resonance::load_wav_audio_asset("assets/audio/bgm/explore.wav");
    const auto missing = resonance::load_wav_audio_asset("assets/audio/bgm/missing.wav");

    resonance::WavAudioAsset bed;
    bed.sample_rate = 48000;
    bed.channels = 2;
    bed.samples = {0.5F, -0.5F, 0.25F, -0.25F};

    resonance::WavAudioAsset layer;
    layer.sample_rate = 48000;
    layer.channels = 2;
    layer.samples = {0.1F, 0.2F, 0.3F, 0.4F};

    std::vector<float> mixed(4U, 0.0F);
    std::size_t bed_cursor = 0;
    std::size_t layer_cursor = 0;

    resonance::mix_looping_audio_asset(bed, bed_cursor, 1.0F, mixed);
    resonance::mix_looping_audio_asset(layer, layer_cursor, 0.5F, mixed);

    const bool loaded_ok = explore.has_value() &&
        explore->sample_rate == 48000 &&
        explore->channels == 2 &&
        !explore->samples.empty() &&
        !missing.has_value();

    const bool mixed_ok = nearly_equal(mixed[0], 0.55F) &&
        nearly_equal(mixed[1], -0.4F) &&
        nearly_equal(mixed[2], 0.4F) &&
        nearly_equal(mixed[3], -0.05F);

    std::vector<float> wrapped(4U, 0.0F);
    resonance::mix_looping_audio_asset(layer, layer_cursor, 1.0F, wrapped);
    const bool wrapped_ok = layer_cursor == 0U &&
        nearly_equal(wrapped[0], 0.1F) &&
        nearly_equal(wrapped[1], 0.2F) &&
        nearly_equal(wrapped[2], 0.3F) &&
        nearly_equal(wrapped[3], 0.4F);

    return (loaded_ok && mixed_ok && wrapped_ok) ? 0 : 1;
}
