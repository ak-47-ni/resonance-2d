#include "engine/event/EventDirector.h"

#include <algorithm>
#include <utility>
#include <vector>

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

std::vector<std::string> missing_world_tags(const EventData& event, const EventContext& context) {
    std::vector<std::string> missing;
    for (const auto& tag : event.required_world_tags) {
        if (!context.world_tags.contains(tag)) {
            missing.push_back(tag);
        }
    }
    return missing;
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

std::vector<std::string> EventDirector::visible_event_ids(const EventContext& context) const {
    std::vector<const EventData*> visible;
    for (const auto& event : events_) {
        if (event.region_id != context.region_id) {
            continue;
        }
        if (!required_tags_satisfied(event, context)) {
            continue;
        }
        visible.push_back(&event);
    }

    std::sort(visible.begin(), visible.end(), [](const EventData* lhs, const EventData* rhs) {
        if (lhs->weight != rhs->weight) {
            return lhs->weight > rhs->weight;
        }
        return lhs->id < rhs->id;
    });

    std::vector<std::string> ids;
    ids.reserve(visible.size());
    for (const auto* event : visible) {
        ids.push_back(event->id);
    }
    return ids;
}

std::optional<LockedEventHint> EventDirector::next_locked_event_hint(const EventContext& context) const {
    const EventData* best_event = nullptr;
    std::vector<std::string> best_missing;

    for (const auto& event : events_) {
        if (event.region_id != context.region_id) {
            continue;
        }

        const auto missing = missing_world_tags(event, context);
        if (missing.empty()) {
            continue;
        }

        if (best_event == nullptr ||
            missing.size() < best_missing.size() ||
            (missing.size() == best_missing.size() && event.weight > best_event->weight) ||
            (missing.size() == best_missing.size() && event.weight == best_event->weight && event.id < best_event->id)) {
            best_event = &event;
            best_missing = missing;
        }
    }

    if (best_event == nullptr) {
        return std::nullopt;
    }

    return LockedEventHint{best_event->id, best_missing};
}

}  // namespace resonance
