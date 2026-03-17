#pragma once

#include "engine/world/RegionData.h"
#include "engine/world/Trigger.h"

#include <string>
#include <vector>

namespace resonance {

class World {
public:
    explicit World(std::vector<RegionData> regions);

    void set_player_position(WorldPosition position);
    WorldPosition player_position() const;
    std::string current_region_id() const;
    std::size_t region_count() const;
    std::vector<RegionData>& regions();
    const std::vector<RegionData>& regions() const;

private:
    std::string resolve_region_for(WorldPosition position) const;

    std::vector<RegionData> regions_;
    WorldPosition player_position_{};
    std::string current_region_id_;
};

}  // namespace resonance
