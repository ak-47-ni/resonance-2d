#pragma once

#include "engine/audio/AudioBackend.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace resonance {

class AudioDirector {
public:
    AudioDirector();
    explicit AudioDirector(std::unique_ptr<AudioBackend> backend);

    void set_region_music(const std::string& region_id, const std::string& music_state);
    void enter_region(const std::string& region_id);
    void request_story_cue(const std::string& music_state);
    void clear_story_cue();

    std::string current_music_state() const;
    std::vector<std::string> current_ambient_layers() const;

private:
    void refresh_snapshot();

    std::unique_ptr<AudioBackend> backend_;
    std::unordered_map<std::string, std::string> region_music_;
    std::unordered_set<std::string> known_music_states_;
    std::string current_region_id_;
    std::string region_music_state_;
    std::string story_override_state_;
    AudioStateSnapshot current_snapshot_;
};

}  // namespace resonance
