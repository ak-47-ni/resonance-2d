#include "engine/audio/AudioDirector.h"

#include <utility>

namespace resonance {
namespace {

std::unordered_set<std::string> default_music_states() {
    return {"explore", "mysterious", "calm"};
}

std::string resolve_audio_track_path(const std::filesystem::path& asset_root, const std::string& bgm_track) {
    if (bgm_track.empty()) {
        return {};
    }
    return (asset_root / bgm_track).lexically_normal().generic_string();
}

std::string resolve_ambient_track_path(const std::filesystem::path& asset_root, const std::string& ambient_layer) {
    if (ambient_layer.empty()) {
        return {};
    }
    return (asset_root / "audio" / "ambient" / (ambient_layer + ".wav")).lexically_normal().generic_string();
}

}  // namespace

AudioDirector::AudioDirector()
    : AudioDirector(make_default_audio_backend()) {}

AudioDirector::AudioDirector(std::unique_ptr<AudioBackend> backend)
    : backend_(std::move(backend)), known_music_states_(default_music_states()) {}

void AudioDirector::register_music_states(
    const std::vector<MusicStateData>& music_states,
    const std::filesystem::path& asset_root
) {
    asset_root_ = asset_root;
    music_states_.clear();
    for (const auto& music_state : music_states) {
        known_music_states_.insert(music_state.id);
        music_states_.insert_or_assign(music_state.id, music_state);
    }
    refresh_snapshot();
}

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
    AudioStateSnapshot next_snapshot;
    if (!story_override_state_.empty()) {
        next_snapshot.music_state = story_override_state_;
    } else {
        next_snapshot.music_state = region_music_state_;
    }

    if (!next_snapshot.music_state.empty()) {
        next_snapshot.fallback_music_state = next_snapshot.music_state;
        const auto music_state_it = music_states_.find(next_snapshot.music_state);
        if (music_state_it != music_states_.end()) {
            next_snapshot.ambient_layers = music_state_it->second.ambient_layers;
            next_snapshot.resolved_bgm_track = resolve_audio_track_path(asset_root_, music_state_it->second.bgm_track);
            for (const auto& ambient_layer : next_snapshot.ambient_layers) {
                const auto resolved_track = resolve_ambient_track_path(asset_root_, ambient_layer);
                if (!resolved_track.empty()) {
                    next_snapshot.resolved_ambient_tracks.push_back(resolved_track);
                }
            }
        }
    }

    current_snapshot_ = std::move(next_snapshot);
    if (backend_) {
        backend_->apply(current_snapshot_);
    }
}

}  // namespace resonance
