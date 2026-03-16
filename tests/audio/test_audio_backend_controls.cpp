#include "engine/audio/AudioBackend.h"

#include <cmath>
#include <memory>
#include <optional>
#include <string>

namespace {

bool nearly_equal(float lhs, float rhs, float epsilon = 0.0001F) {
    return std::fabs(lhs - rhs) <= epsilon;
}

bool has_state(const std::optional<resonance::AudioBackendDebugState>& state) {
    return state.has_value();
}

}  // namespace

int main() {
    auto backend = resonance::make_default_audio_backend();

    resonance::AudioStateSnapshot initial;
    initial.music_state = "explore";
    initial.resolved_bgm_track = "assets/audio/bgm/explore.wav";
    initial.fallback_music_state = "explore";
    initial.master_volume = 1.4F;
    initial.bgm_volume = -0.25F;
    initial.ambient_volume = 0.35F;
    initial.crossfade_seconds = -2.0F;
    backend->apply(initial);

    const auto clamped = backend->debug_state_for_tests();
    const bool clamped_ok = has_state(clamped) &&
        clamped->primary_track == "assets/audio/bgm/explore.wav" &&
        clamped->fading_track.empty() &&
        nearly_equal(clamped->master_volume, 1.0F) &&
        nearly_equal(clamped->bgm_volume, 0.0F) &&
        nearly_equal(clamped->ambient_volume, 0.35F) &&
        nearly_equal(clamped->crossfade_seconds, 0.0F) &&
        !clamped->crossfade_active;

    resonance::AudioStateSnapshot same_track = initial;
    same_track.master_volume = 0.9F;
    same_track.bgm_volume = 0.4F;
    same_track.ambient_volume = 0.6F;
    same_track.crossfade_seconds = 1.5F;
    backend->apply(same_track);

    const auto same_track_state = backend->debug_state_for_tests();
    const bool same_track_ok = has_state(same_track_state) &&
        same_track_state->primary_track == "assets/audio/bgm/explore.wav" &&
        same_track_state->fading_track.empty() &&
        nearly_equal(same_track_state->master_volume, 0.9F) &&
        nearly_equal(same_track_state->bgm_volume, 0.4F) &&
        nearly_equal(same_track_state->ambient_volume, 0.6F) &&
        nearly_equal(same_track_state->crossfade_seconds, 1.5F) &&
        !same_track_state->crossfade_active;

    resonance::AudioStateSnapshot immediate_switch = same_track;
    immediate_switch.music_state = "mysterious";
    immediate_switch.resolved_bgm_track = "assets/audio/bgm/mysterious.wav";
    immediate_switch.fallback_music_state = "mysterious";
    immediate_switch.crossfade_seconds = 0.0F;
    backend->apply(immediate_switch);

    const auto immediate_state = backend->debug_state_for_tests();
    const bool immediate_ok = has_state(immediate_state) &&
        immediate_state->primary_track == "assets/audio/bgm/mysterious.wav" &&
        immediate_state->fading_track.empty() &&
        !immediate_state->crossfade_active;

    resonance::AudioStateSnapshot crossfade = immediate_switch;
    crossfade.music_state = "calm";
    crossfade.resolved_bgm_track = "assets/audio/bgm/calm.wav";
    crossfade.fallback_music_state = "calm";
    crossfade.crossfade_seconds = 0.5F;
    backend->apply(crossfade);

    const auto crossfade_start = backend->debug_state_for_tests();
    const bool crossfade_start_ok = has_state(crossfade_start) &&
        crossfade_start->primary_track == "assets/audio/bgm/calm.wav" &&
        crossfade_start->fading_track == "assets/audio/bgm/mysterious.wav" &&
        crossfade_start->crossfade_active &&
        nearly_equal(crossfade_start->crossfade_elapsed_seconds, 0.0F) &&
        nearly_equal(crossfade_start->crossfade_seconds, 0.5F);

    backend->advance_time_for_tests(0.25F);
    const auto crossfade_mid = backend->debug_state_for_tests();
    const bool crossfade_mid_ok = has_state(crossfade_mid) &&
        crossfade_mid->crossfade_active &&
        nearly_equal(crossfade_mid->crossfade_elapsed_seconds, 0.25F);

    backend->advance_time_for_tests(0.30F);
    const auto crossfade_done = backend->debug_state_for_tests();
    const bool crossfade_done_ok = has_state(crossfade_done) &&
        crossfade_done->primary_track == "assets/audio/bgm/calm.wav" &&
        crossfade_done->fading_track.empty() &&
        !crossfade_done->crossfade_active &&
        nearly_equal(crossfade_done->crossfade_elapsed_seconds, 0.0F);

    return (clamped_ok && same_track_ok && immediate_ok && crossfade_start_ok && crossfade_mid_ok && crossfade_done_ok) ? 0 : 1;
}
