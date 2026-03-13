#include "engine/event/EventDirector.h"

int main() {
    const auto bundle = resonance::load_demo_content("assets/data");
    resonance::EventDirector director(bundle.events);

    const auto result = director.pick_event({"ruins", {}, 120.0});
    const auto no_result = director.pick_event({"unknown_region", {}, 120.0});

    return (result.has_value() && result->id == "old_broadcast_echo" && !no_result.has_value()) ? 0 : 1;
}
