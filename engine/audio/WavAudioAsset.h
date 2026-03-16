#pragma once

#include <cstddef>
#include <filesystem>
#include <optional>
#include <span>
#include <vector>

namespace resonance {

struct WavAudioAsset {
    int sample_rate = 0;
    int channels = 0;
    std::vector<float> samples;
};

std::optional<WavAudioAsset> load_wav_audio_asset(const std::filesystem::path& path);
void mix_looping_audio_asset(const WavAudioAsset& asset, std::size_t& cursor, float gain, std::span<float> target);

}  // namespace resonance
