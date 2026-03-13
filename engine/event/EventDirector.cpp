#include "engine/event/EventDirector.h"

#include <utility>

namespace resonance {

EventDirector::EventDirector(std::vector<EventData> events)
    : events_(std::move(events)) {}

std::optional<EventData> EventDirector::pick_event(const EventContext& context) const {
    const EventData* best_match = nullptr;

    for (const auto& event : events_) {
        if (event.region_id != context.region_id) {
            continue;
        }

        if (best_match == nullptr || event.weight > best_match->weight) {
            best_match = &event;
        }
    }

    if (best_match == nullptr) {
        return std::nullopt;
    }

    return *best_match;
}

}  // namespace resonance
