#include "engine/event/EventDirector.h"

#include <utility>

namespace resonance {
namespace {

constexpr double kMajorEventCooldownSeconds = 8.0;

bool required_tags_satisfied(const EventData& event, const EventContext& context) {
    for (const auto& tag : event.required_world_tags) {
        if (!context.world_tags.contains(tag)) {
            return false;
        }
    }
    return true;
}

const EventData* find_best_match(
    const std::vector<EventData>& events,
    const EventContext& context,
    bool exclude_recent) {
    const EventData* best_match = nullptr;

    for (const auto& event : events) {
        if (event.region_id != context.region_id) {
            continue;
        }
        if (!required_tags_satisfied(event, context)) {
            continue;
        }
        if (exclude_recent && context.recent_event_ids.contains(event.id)) {
            continue;
        }
        if (best_match == nullptr || event.weight > best_match->weight) {
            best_match = &event;
        }
    }

    return best_match;
}

}  // namespace

EventDirector::EventDirector(std::vector<EventData> events)
    : events_(std::move(events)) {}

std::optional<EventData> EventDirector::pick_event(const EventContext& context) const {
    if (context.seconds_since_last_major_event < kMajorEventCooldownSeconds) {
        return std::nullopt;
    }

    if (const auto* best_match = find_best_match(events_, context, true); best_match != nullptr) {
        return *best_match;
    }

    if (const auto* fallback = find_best_match(events_, context, false); fallback != nullptr) {
        return *fallback;
    }

    return std::nullopt;
}

}  // namespace resonance
