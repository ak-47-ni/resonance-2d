#include "engine/audio/AudioDirector.h"

#include <utility>

namespace resonance {
namespace {

std::unordered_set<std::string> default_music_states() {
    return {"explore", "mysterious", "calm"};
}

}  // namespace

AudioDirector::AudioDirector()
    : AudioDirector(make_default_audio_backend()) {}

AudioDirector::AudioDirector(std::unique_ptr<AudioBackend> backend)
    : backend_(std::move(backend)), known_music_states_(default_music_states()) {}

void AudioDirector::set_region_music(const std::string& region_id, const std::string& music_state) {
    region_music_[region_id] = music_state;
    known_music_states_.insert(music_state);
}

void AudioDirector::enter_region(const std::string& region_id) {
    const auto region_it = region_music_.find(region_id);
    if (region_it == region_music_.end()) {
        current_region_id_.clear();
        region_music_state_.clear();
        refresh_snapshot();
        return;
    }

    current_region_id_ = region_id;
    region_music_state_ = region_it->second;
    refresh_snapshot();
}

void AudioDirector::request_story_cue(const std::string& music_state) {
    if (!known_music_states_.contains(music_state)) {
        return;
    }

    story_override_state_ = music_state;
    refresh_snapshot();
}

void AudioDirector::clear_story_cue() {
    story_override_state_.clear();
    refresh_snapshot();
}

std::string AudioDirector::current_music_state() const {
    return current_snapshot_.music_state;
}

std::vector<std::string> AudioDirector::current_ambient_layers() const {
    return current_snapshot_.ambient_layers;
}

void AudioDirector::refresh_snapshot() {
    if (!story_override_state_.empty()) {
        current_snapshot_.music_state = story_override_state_;
    } else {
        current_snapshot_.music_state = region_music_state_;
    }

    if (backend_) {
        backend_->apply(current_snapshot_);
    }
}

}  // namespace resonance
