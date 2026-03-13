#include "engine/audio/AudioBackend.h"

#include "engine/audio/AudioToneProfile.h"

#include <cmath>
#include <memory>
#include <mutex>
#include <utility>
#include <vector>

#if RESONANCE_HAS_SDL
#include <SDL3/SDL.h>
#include <SDL3/SDL_audio.h>
#endif

namespace resonance {
namespace {

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
        profile_ = make_tone_profile(snapshot);
#if RESONANCE_HAS_SDL
        ensure_stream_locked();
#endif
    }

private:
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
        if (profile_.master_gain > 0.0F) {
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

        SDL_PutAudioStreamData(stream, samples.data(), static_cast<int>(samples.size() * sizeof(float)));
    }
#endif

    std::mutex mutex_;
    AudioStateSnapshot last_snapshot_;
    AudioToneProfile profile_;
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
