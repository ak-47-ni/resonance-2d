#include "engine/event/EventDirector.h"

#include <string>
#include <vector>

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

    const auto meadow_visible = director.visible_event_ids({"meadow", {}, 120.0, {}});
    const auto meadow_unlock = director.next_locked_event_hint({"meadow", {}, 120.0, {}});
    const auto station_one_visible = director.visible_event_ids({"station", {"memory:meadow-swing"}, 120.0, {}});
    const auto station_one_unlock = director.next_locked_event_hint({"station", {"memory:meadow-swing"}, 120.0, {}});
    const auto station_two_visible = director.visible_event_ids({"station", {"memory:meadow-swing", "memory:ruins-gate"}, 120.0, {}});
    const auto station_two_unlock = director.next_locked_event_hint({"station", {"memory:meadow-swing", "memory:ruins-gate"}, 120.0, {}});
    const auto station_three_visible = director.visible_event_ids({"station", {"memory:meadow-swing", "memory:ruins-gate", "memory:lakeside-reeds"}, 120.0, {}});
    const auto station_three_unlock = director.next_locked_event_hint({"station", {"memory:meadow-swing", "memory:ruins-gate", "memory:lakeside-reeds"}, 120.0, {}});

    const bool meadow_visible_ok = meadow_visible == std::vector<std::string>{"distant_bell", "passing_shadow"};
    const bool meadow_unlock_ok = meadow_unlock.has_value() &&
        meadow_unlock->event_id == "swing_memory_echo" &&
        meadow_unlock->missing_world_tags == std::vector<std::string>{"memory:meadow-swing"};

    const bool station_one_visible_ok = station_one_visible == std::vector<std::string>{"echoing_announcement"};
    const bool station_one_unlock_ok = station_one_unlock.has_value() &&
        station_one_unlock->event_id == "platform_convergence" &&
        station_one_unlock->missing_world_tags == std::vector<std::string>{"memory:ruins-gate"};

    const bool station_two_visible_ok = station_two_visible == std::vector<std::string>{"platform_convergence", "echoing_announcement"};
    const bool station_two_unlock_ok = station_two_unlock.has_value() &&
        station_two_unlock->event_id == "terminal_refrain" &&
        station_two_unlock->missing_world_tags == std::vector<std::string>{"memory:lakeside-reeds"};

    const bool station_three_visible_ok = station_three_visible == std::vector<std::string>{"terminal_refrain", "platform_convergence", "echoing_announcement"};
    const bool station_three_unlock_ok = !station_three_unlock.has_value();

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
            !no_result.has_value() &&
            meadow_visible_ok &&
            meadow_unlock_ok &&
            station_one_visible_ok &&
            station_one_unlock_ok &&
            station_two_visible_ok &&
            station_two_unlock_ok &&
            station_three_visible_ok &&
            station_three_unlock_ok)
        ? 0
        : 1;
}
