#include "engine/event/EventData.h"

int main() {
    const auto bundle = resonance::load_demo_content("assets/data");
    return (bundle.regions.size() >= 4 && bundle.music_states.size() >= 3 && bundle.events.size() >= 6) ? 0 : 1;
}
