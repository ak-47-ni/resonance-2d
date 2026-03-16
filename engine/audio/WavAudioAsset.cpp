#include "engine/audio/WavAudioAsset.h"

#if RESONANCE_HAS_SDL
#include <SDL3/SDL.h>
#include <SDL3/SDL_audio.h>
#endif

#include <algorithm>
#include <cstring>
#include <string>

namespace resonance {

std::optional<WavAudioAsset> load_wav_audio_asset(const std::filesystem::path& path) {
    if (path.empty()) {
        return std::nullopt;
    }

#if RESONANCE_HAS_SDL
    SDL_AudioSpec source_spec{};
    Uint8* source_buffer = nullptr;
    Uint32 source_length = 0;
    if (!SDL_LoadWAV(path.string().c_str(), &source_spec, &source_buffer, &source_length)) {
        return std::nullopt;
    }

    SDL_AudioSpec target_spec{
        SDL_AUDIO_F32,
        2,
        48000,
    };

    Uint8* converted_buffer = nullptr;
    int converted_length = 0;
    const bool converted = SDL_ConvertAudioSamples(
        &source_spec,
        source_buffer,
        static_cast<int>(source_length),
        &target_spec,
        &converted_buffer,
        &converted_length
    );
    SDL_free(source_buffer);
    if (!converted) {
        return std::nullopt;
    }

    WavAudioAsset asset;
    asset.sample_rate = target_spec.freq;
    asset.channels = static_cast<int>(target_spec.channels);
    const std::size_t sample_count = static_cast<std::size_t>(converted_length) / sizeof(float);
    asset.samples.resize(sample_count);
    std::memcpy(asset.samples.data(), converted_buffer, sample_count * sizeof(float));
    SDL_free(converted_buffer);
    return asset;
#else
    return std::nullopt;
#endif
}

void mix_looping_audio_asset(const WavAudioAsset& asset, std::size_t& cursor, float gain, std::span<float> target) {
    if (asset.channels != 2 || asset.samples.empty() || target.empty() || gain == 0.0F) {
        return;
    }

    const std::size_t initial_cursor = cursor;
    for (std::size_t index = 0; index < target.size(); ++index) {
        target[index] += asset.samples[cursor] * gain;
        cursor = (cursor + 1U) % asset.samples.size();
    }

    if (asset.samples.size() <= target.size() && initial_cursor == 0U) {
        cursor %= asset.samples.size();
    }
}

}  // namespace resonance
