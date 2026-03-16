#include "engine/event/EventDirector.h"

int main() {
    const auto bundle = resonance::load_demo_content("assets/data");
    resonance::EventDirector director(bundle.events);

    const auto first = director.pick_event({"meadow", {}, 120.0, {}});
    const auto cooldown_blocked = director.pick_event({"meadow", {}, 2.0, {"distant_bell"}});
    const auto rotated = director.pick_event({"meadow", {}, 120.0, {"distant_bell"}});
    const auto gated_visible = director.pick_event({"meadow", {"memory:meadow-swing"}, 120.0, {"distant_bell"}});
    const auto multi_single_memory = director.pick_event({"station", {"memory:meadow-swing"}, 120.0, {}});
    const auto multi_visible = director.pick_event({"station", {"memory:meadow-swing", "memory:ruins-gate"}, 120.0, {}});
    const auto triple_visible = director.pick_event({"station", {"memory:meadow-swing", "memory:ruins-gate", "memory:lakeside-reeds"}, 120.0, {}});
    const auto no_result = director.pick_event({"unknown_region", {}, 120.0, {}});

    return (first.has_value() &&
            first->id == "distant_bell" &&
            !cooldown_blocked.has_value() &&
            rotated.has_value() &&
            rotated->id == "passing_shadow" &&
            gated_visible.has_value() &&
            gated_visible->id == "swing_memory_echo" &&
            multi_single_memory.has_value() &&
            multi_single_memory->id == "echoing_announcement" &&
            multi_visible.has_value() &&
            multi_visible->id == "platform_convergence" &&
            triple_visible.has_value() &&
            triple_visible->id == "terminal_refrain" &&
            !no_result.has_value())
        ? 0
        : 1;
}
