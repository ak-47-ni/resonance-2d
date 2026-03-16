#pragma once

#include <string>
#include <unordered_set>

namespace resonance {

struct EventContext {
    std::string region_id;
    std::unordered_set<std::string> world_tags;
    double seconds_since_last_major_event = 0.0;
    std::unordered_set<std::string> recent_event_ids;
};

}  // namespace resonance
