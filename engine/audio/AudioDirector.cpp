#include "engine/audio/AudioDirector.h"

#include <algorithm>
#include <utility>

namespace resonance {
namespace {

std::unordered_set<std::string> default_music_states() {
    return {"explore", "mysterious", "calm"};
}

constexpr float kMaxBgmDuck = 0.2F;
constexpr float kMaxAmbientBoost = 0.6F;
constexpr float kMaxEventDuck = 0.12F;
constexpr float kMaxEventAmbientBoost = 0.4F;

float clamp_unit(float value) {
    return std::clamp(value, 0.0F, 1.0F);
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

void AudioDirector::set_story_focus(float focus) {
    const float clamped_focus = clamp_unit(focus);
    if (clamped_focus == story_focus_) {
        return;
    }

    story_focus_ = clamped_focus;
    refresh_snapshot();
}

void AudioDirector::set_event_emphasis(float emphasis) {
    const float clamped_emphasis = clamp_unit(emphasis);
    if (clamped_emphasis == event_emphasis_) {
        return;
    }

    event_emphasis_ = clamped_emphasis;
    refresh_snapshot();
}

void AudioDirector::set_event_mix(float event_duck, float ambient_boost) {
    const float clamped_event_duck = clamp_unit(event_duck);
    const float clamped_ambient_boost = clamp_unit(ambient_boost);
    if (clamped_event_duck == event_duck_scalar_ && clamped_ambient_boost == ambient_boost_scalar_) {
        return;
    }

    event_duck_scalar_ = clamped_event_duck;
    ambient_boost_scalar_ = clamped_ambient_boost;
    refresh_snapshot();
}

std::string AudioDirector::current_music_state() const {
    return current_snapshot_.music_state;
}

std::vector<std::string> AudioDirector::current_ambient_layers() const {
    return current_snapshot_.ambient_layers;
}

float AudioDirector::current_bgm_gain() const {
    return current_snapshot_.bgm_gain;
}

float AudioDirector::current_ambient_gain_multiplier() const {
    return current_snapshot_.ambient_gain_multiplier;
}

void AudioDirector::refresh_snapshot() {
    AudioStateSnapshot next_snapshot;
    next_snapshot.story_focus = story_focus_;
    next_snapshot.event_emphasis = event_emphasis_;
    next_snapshot.bgm_gain = std::clamp(
        1.0F - (kMaxBgmDuck * story_focus_) - (kMaxEventDuck * event_emphasis_ * event_duck_scalar_),
        0.0F,
        1.0F
    );
    next_snapshot.ambient_gain_multiplier =
        1.0F + (kMaxAmbientBoost * story_focus_) + (kMaxEventAmbientBoost * event_emphasis_ * ambient_boost_scalar_);
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
