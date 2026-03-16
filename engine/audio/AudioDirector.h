#pragma once

#include "engine/audio/AudioBackend.h"
#include "engine/audio/MusicStateData.h"

#include <filesystem>
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

    void register_music_states(
        const std::vector<MusicStateData>& music_states,
        const std::filesystem::path& asset_root = "assets"
    );
    void set_region_music(const std::string& region_id, const std::string& music_state);
    void enter_region(const std::string& region_id);
    void request_story_cue(const std::string& music_state);
    void set_story_focus(float focus);
    void set_event_emphasis(float emphasis);
    void set_event_mix(float event_duck, float ambient_boost);
    void clear_story_cue();

    std::string current_music_state() const;
    std::vector<std::string> current_ambient_layers() const;
    float current_bgm_gain() const;
    float current_ambient_gain_multiplier() const;

private:
    void refresh_snapshot();

    std::unique_ptr<AudioBackend> backend_;
    std::filesystem::path asset_root_{"assets"};
    std::unordered_map<std::string, std::string> region_music_;
    std::unordered_map<std::string, MusicStateData> music_states_;
    std::unordered_set<std::string> known_music_states_;
    std::string current_region_id_;
    std::string region_music_state_;
    std::string story_override_state_;
    float story_focus_ = 0.0F;
    float event_emphasis_ = 0.0F;
    float event_duck_scalar_ = 1.0F;
    float ambient_boost_scalar_ = 1.0F;
    AudioStateSnapshot current_snapshot_;
};

}  // namespace resonance
