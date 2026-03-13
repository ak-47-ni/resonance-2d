#pragma once

#include <memory>
#include <string>
#include <vector>

namespace resonance {

struct AudioStateSnapshot {
    std::string music_state;
    std::vector<std::string> ambient_layers;
};

class AudioBackend {
public:
    virtual ~AudioBackend() = default;

    virtual void apply(const AudioStateSnapshot& snapshot) = 0;
};

std::unique_ptr<AudioBackend> make_default_audio_backend();

}  // namespace resonance
