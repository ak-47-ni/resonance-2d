#include "engine/event/EventData.h"

int main() {
    const auto bundle = resonance::load_demo_content("assets/data");
    return (!bundle.regions.empty() && !bundle.music_states.empty() && !bundle.events.empty()) ? 0 : 1;
}
