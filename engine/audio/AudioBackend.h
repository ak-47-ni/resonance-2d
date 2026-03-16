#pragma once

#include <memory>
#include <string>
#include <vector>

namespace resonance {

struct AudioStateSnapshot {
    std::string music_state;
    std::string resolved_bgm_track;
    std::string fallback_music_state;
    std::vector<std::string> ambient_layers;
    std::vector<std::string> resolved_ambient_tracks;
};

class AudioBackend {
public:
    virtual ~AudioBackend() = default;

    virtual void apply(const AudioStateSnapshot& snapshot) = 0;
};

std::unique_ptr<AudioBackend> make_default_audio_backend();

}  // namespace resonance
