#pragma once

#include "engine/event/EventContext.h"
#include "engine/event/EventData.h"

#include <optional>
#include <vector>

namespace resonance {

class EventDirector {
public:
    explicit EventDirector(std::vector<EventData> events);

    std::optional<EventData> pick_event(const EventContext& context) const;

private:
    std::vector<EventData> events_;
};

}  // namespace resonance
