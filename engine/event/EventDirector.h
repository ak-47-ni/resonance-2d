#pragma once

#include "engine/event/EventContext.h"
#include "engine/event/EventData.h"

#include <optional>
#include <vector>

namespace resonance {

struct LockedEventHint {
    std::string event_id;
    std::vector<std::string> missing_world_tags;
};

class EventDirector {
public:
    explicit EventDirector(std::vector<EventData> events);

    std::optional<EventData> pick_event(const EventContext& context) const;
    std::vector<std::string> visible_event_ids(const EventContext& context) const;
    std::optional<LockedEventHint> next_locked_event_hint(const EventContext& context) const;

private:
    std::vector<EventData> events_;
};

}  // namespace resonance
