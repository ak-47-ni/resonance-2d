#include "engine/world/World.h"

#include <utility>

namespace resonance {

World::World(std::vector<RegionData> regions)
    : regions_(std::move(regions)) {}

void World::set_player_position(WorldPosition position) {
    player_position_ = position;
    current_region_id_ = resolve_region_for(position);
}

WorldPosition World::player_position() const {
    return player_position_;
}

std::string World::current_region_id() const {
    return current_region_id_;
}


std::size_t World::region_count() const {
    return regions_.size();
}

const std::vector<RegionData>& World::regions() const {
    return regions_;
}
std::string World::resolve_region_for(WorldPosition position) const {
    for (const auto& region : regions_) {
        const Trigger trigger(
            region.bounds.x,
            region.bounds.y,
            region.bounds.width,
            region.bounds.height
        );
        if (trigger.contains(position)) {
            return region.id;
        }
    }
    return {};
}

}  // namespace resonance
