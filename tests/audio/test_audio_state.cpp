#include "engine/audio/AudioDirector.h"

int main() {
    resonance::AudioDirector audio;

    audio.set_region_music("meadow", "explore");
    audio.enter_region("meadow");
    const auto initial = audio.current_music_state();

    audio.request_story_cue("mysterious");
    const auto overridden = audio.current_music_state();

    audio.enter_region("unknown_region");
    const auto after_unknown = audio.current_music_state();

    audio.request_story_cue("unknown_state");
    const auto after_invalid_cue = audio.current_music_state();

    return (initial == "explore" &&
            overridden == "mysterious" &&
            after_unknown == "mysterious" &&
            after_invalid_cue == "mysterious")
        ? 0
        : 1;
}
