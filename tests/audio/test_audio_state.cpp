#include "engine/audio/AudioDirector.h"
#include "engine/audio/MusicStateData.h"

#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace {

class CaptureBackend final : public resonance::AudioBackend {
public:
    void apply(const resonance::AudioStateSnapshot& snapshot) override {
        snapshots.push_back(snapshot);
        last_snapshot = snapshot;
    }

    resonance::AudioStateSnapshot last_snapshot;
    std::vector<resonance::AudioStateSnapshot> snapshots;
};

}  // namespace

int main() {
    auto backend = std::make_unique<CaptureBackend>();
    auto* capture = backend.get();
    resonance::AudioDirector audio(std::move(backend));

    audio.register_music_states({
        resonance::MusicStateData{"explore", "audio/bgm/explore.wav", {"wind", "water"}},
        resonance::MusicStateData{"mysterious", "audio/bgm/mysterious.wav", {"rumble"}},
        resonance::MusicStateData{"missing_asset", "audio/bgm/missing.wav", {"dust"}},
    });

    audio.set_region_music("meadow", "explore");
    audio.set_region_music("ruin", "missing_asset");

    audio.enter_region("meadow");
    const auto initial = capture->last_snapshot;

    audio.request_story_cue("mysterious");
    const auto overridden = capture->last_snapshot;

    audio.clear_story_cue();
    audio.enter_region("ruin");
    const auto missing_asset = capture->last_snapshot;

    audio.enter_region("unknown_region");
    const auto after_unknown = capture->last_snapshot;

    audio.request_story_cue("unknown_state");
    const auto after_invalid_cue = capture->last_snapshot;

    const bool initial_ok = initial.music_state == "explore" &&
        initial.resolved_bgm_track == "assets/audio/bgm/explore.wav" &&
        initial.fallback_music_state == "explore" &&
        initial.ambient_layers == std::vector<std::string>{"wind", "water"} &&
        initial.resolved_ambient_tracks == std::vector<std::string>{
            "assets/audio/ambient/wind.wav",
            "assets/audio/ambient/water.wav",
        };

    const bool overridden_ok = overridden.music_state == "mysterious" &&
        overridden.resolved_bgm_track == "assets/audio/bgm/mysterious.wav" &&
        overridden.fallback_music_state == "mysterious" &&
        overridden.ambient_layers == std::vector<std::string>{"rumble"} &&
        overridden.resolved_ambient_tracks == std::vector<std::string>{
            "assets/audio/ambient/rumble.wav",
        };

    const bool missing_ok = missing_asset.music_state == "missing_asset" &&
        missing_asset.resolved_bgm_track == "assets/audio/bgm/missing.wav" &&
        missing_asset.fallback_music_state == "missing_asset" &&
        missing_asset.ambient_layers == std::vector<std::string>{"dust"} &&
        missing_asset.resolved_ambient_tracks == std::vector<std::string>{
            "assets/audio/ambient/dust.wav",
        };

    const bool unknown_ok = after_unknown.music_state.empty() &&
        after_unknown.resolved_bgm_track.empty() &&
        after_unknown.fallback_music_state.empty() &&
        after_unknown.ambient_layers.empty() &&
        after_unknown.resolved_ambient_tracks.empty();

    const bool invalid_cue_ok = after_invalid_cue.music_state.empty() &&
        after_invalid_cue.resolved_bgm_track.empty() &&
        after_invalid_cue.fallback_music_state.empty() &&
        after_invalid_cue.resolved_ambient_tracks.empty();

    return (initial_ok && overridden_ok && missing_ok && unknown_ok && invalid_cue_ok) ? 0 : 1;
}
