#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace resonance {

struct AudioStateSnapshot {
    std::string music_state;
    std::string resolved_bgm_track;
    std::string fallback_music_state;
    std::vector<std::string> ambient_layers;
    std::vector<std::string> resolved_ambient_tracks;
    float story_focus = 0.0F;
    float event_emphasis = 0.0F;
    float bgm_gain = 1.0F;
    float ambient_gain_multiplier = 1.0F;
    float master_volume = 1.0F;
    float bgm_volume = 1.0F;
    float ambient_volume = 1.0F;
    float crossfade_seconds = 1.0F;
};

struct AudioBackendDebugState {
    std::string primary_track;
    std::string fading_track;
    float master_volume = 1.0F;
    float bgm_volume = 1.0F;
    float ambient_volume = 1.0F;
    float crossfade_seconds = 1.0F;
    float crossfade_elapsed_seconds = 0.0F;
    bool crossfade_active = false;
};

class AudioBackend {
public:
    virtual ~AudioBackend() = default;

    virtual void apply(const AudioStateSnapshot& snapshot) = 0;

    virtual std::optional<AudioBackendDebugState> debug_state_for_tests() const {
        return std::nullopt;
    }

    virtual void advance_time_for_tests(float seconds) {
        (void)seconds;
    }
};

std::unique_ptr<AudioBackend> make_default_audio_backend();

}  // namespace resonance
