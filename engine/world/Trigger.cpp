#include "engine/world/Trigger.h"

namespace resonance {

Trigger::Trigger(float x, float y, float width, float height)
    : x_(x), y_(y), width_(width), height_(height) {}

bool Trigger::contains(WorldPosition position) const {
    const bool within_x = position.x >= x_ && position.x < (x_ + width_);
    const bool within_y = position.y >= y_ && position.y < (y_ + height_);
    return within_x && within_y;
}

}  // namespace resonance
