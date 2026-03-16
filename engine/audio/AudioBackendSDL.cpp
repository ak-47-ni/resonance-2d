#include "engine/audio/AudioBackend.h"

#include "engine/audio/AudioToneProfile.h"
#include "engine/audio/WavAudioAsset.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <memory>
#include <mutex>
#include <optional>
#include <utility>
#include <vector>

#if RESONANCE_HAS_SDL
#include <SDL3/SDL.h>
#include <SDL3/SDL_audio.h>
#endif

namespace resonance {
namespace {

constexpr float kBgmGain = 0.82F;
constexpr std::array<float, 2> kAmbientGains{0.18F, 0.12F};

class AudioBackendSDL final : public AudioBackend {
public:
    ~AudioBackendSDL() override {
#if RESONANCE_HAS_SDL
        if (stream_ != nullptr) {
            SDL_DestroyAudioStream(stream_);
            stream_ = nullptr;
        }
#endif
    }

    void apply(const AudioStateSnapshot& snapshot) override {
        std::scoped_lock lock(mutex_);
        last_snapshot_ = snapshot;
        AudioStateSnapshot fallback_snapshot;
        fallback_snapshot.music_state = snapshot.fallback_music_state;
        fallback_snapshot.ambient_layers = snapshot.ambient_layers;
        profile_ = make_tone_profile(fallback_snapshot);
        load_assets_if_needed_locked();
#if RESONANCE_HAS_SDL
        ensure_stream_locked();
#endif
    }

private:
    void load_primary_asset_if_needed_locked() {
        if (loaded_track_ == last_snapshot_.resolved_bgm_track) {
            return;
        }

        loaded_track_ = last_snapshot_.resolved_bgm_track;
        loaded_asset_.reset();
        asset_cursor_ = 0;
        if (!loaded_track_.empty()) {
            loaded_asset_ = load_wav_audio_asset(loaded_track_);
        }
    }

    void load_ambient_assets_if_needed_locked() {
        std::vector<std::string> desired_tracks;
        const std::size_t desired_count = std::min(last_snapshot_.resolved_ambient_tracks.size(), kAmbientGains.size());
        desired_tracks.reserve(desired_count);
        for (std::size_t index = 0; index < desired_count; ++index) {
            desired_tracks.push_back(last_snapshot_.resolved_ambient_tracks[index]);
        }

        if (loaded_ambient_tracks_ == desired_tracks) {
            return;
        }

        loaded_ambient_tracks_ = std::move(desired_tracks);
        ambient_assets_.clear();
        ambient_cursors_.clear();
        ambient_assets_.reserve(loaded_ambient_tracks_.size());
        ambient_cursors_.reserve(loaded_ambient_tracks_.size());
        for (const auto& ambient_track : loaded_ambient_tracks_) {
            ambient_assets_.push_back(load_wav_audio_asset(ambient_track));
            ambient_cursors_.push_back(0U);
        }
    }

    void load_assets_if_needed_locked() {
        load_primary_asset_if_needed_locked();
        load_ambient_assets_if_needed_locked();
    }

#if RESONANCE_HAS_SDL
    static void SDLCALL feed_audio(void* userdata, SDL_AudioStream* stream, int additional_amount, int) {
        auto* self = static_cast<AudioBackendSDL*>(userdata);
        self->feed_audio_locked(stream, additional_amount);
    }

    void ensure_stream_locked() {
        if (stream_ != nullptr || stream_failed_) {
            return;
        }
        if ((SDL_WasInit(SDL_INIT_AUDIO) & SDL_INIT_AUDIO) == 0) {
            return;
        }

        const SDL_AudioSpec spec{
            SDL_AUDIO_F32,
            2,
            48000,
        };

        stream_ = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, &AudioBackendSDL::feed_audio, this);
        if (stream_ == nullptr) {
            stream_failed_ = true;
            return;
        }

        if (!SDL_ResumeAudioStreamDevice(stream_)) {
            SDL_DestroyAudioStream(stream_);
            stream_ = nullptr;
            stream_failed_ = true;
        }
    }

    void fill_tone_samples_locked(std::vector<float>& samples, int sample_count) {
        if (profile_.master_gain <= 0.0F) {
            return;
        }

        constexpr float two_pi = 6.28318530717958647692F;
        constexpr float sample_rate = 48000.0F;

        for (int index = 0; index < sample_count; ++index) {
            const float carrier = std::sinf(carrier_phase_) * profile_.master_gain;
            const float layer = std::sinf(layer_phase_) * profile_.layer_gain;
            const float value = carrier + layer;
            const std::size_t sample_index = static_cast<std::size_t>(index) * 2U;
            samples[sample_index] = value;
            samples[sample_index + 1U] = value;

            carrier_phase_ += two_pi * profile_.carrier_frequency / sample_rate;
            layer_phase_ += two_pi * profile_.layer_frequency / sample_rate;
            if (carrier_phase_ > two_pi) {
                carrier_phase_ -= two_pi;
            }
            if (layer_phase_ > two_pi) {
                layer_phase_ -= two_pi;
            }
        }
    }

    void mix_primary_and_ambient_locked(std::vector<float>& samples, int sample_count) {
        if (loaded_asset_.has_value() && !loaded_asset_->samples.empty()) {
            mix_looping_audio_asset(*loaded_asset_, asset_cursor_, kBgmGain, samples);
        } else {
            fill_tone_samples_locked(samples, sample_count);
        }

        for (std::size_t index = 0; index < ambient_assets_.size(); ++index) {
            const auto& ambient_asset = ambient_assets_[index];
            if (!ambient_asset.has_value() || ambient_asset->samples.empty()) {
                continue;
            }
            mix_looping_audio_asset(*ambient_asset, ambient_cursors_[index], kAmbientGains[index], samples);
        }

        for (auto& sample : samples) {
            sample = std::clamp(sample, -1.0F, 1.0F);
        }
    }

    void feed_audio_locked(SDL_AudioStream* stream, int additional_amount) {
        std::scoped_lock lock(mutex_);
        if (additional_amount <= 0) {
            return;
        }

        const int sample_count = additional_amount / static_cast<int>(sizeof(float) * 2);
        if (sample_count <= 0) {
            return;
        }

        std::vector<float> samples(static_cast<std::size_t>(sample_count) * 2U, 0.0F);
        mix_primary_and_ambient_locked(samples, sample_count);
        SDL_PutAudioStreamData(stream, samples.data(), static_cast<int>(samples.size() * sizeof(float)));
    }
#endif

    std::mutex mutex_;
    AudioStateSnapshot last_snapshot_;
    AudioToneProfile profile_;
    std::string loaded_track_;
    std::optional<WavAudioAsset> loaded_asset_;
    std::size_t asset_cursor_ = 0;
    std::vector<std::string> loaded_ambient_tracks_;
    std::vector<std::optional<WavAudioAsset>> ambient_assets_;
    std::vector<std::size_t> ambient_cursors_;
#if RESONANCE_HAS_SDL
    SDL_AudioStream* stream_ = nullptr;
    bool stream_failed_ = false;
    float carrier_phase_ = 0.0F;
    float layer_phase_ = 0.0F;
#endif
};

}  // namespace

std::unique_ptr<AudioBackend> make_default_audio_backend() {
    return std::make_unique<AudioBackendSDL>();
}

}  // namespace resonance
