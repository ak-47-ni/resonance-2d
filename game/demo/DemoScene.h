#pragma once

#include "engine/audio/AudioDirector.h"
#include "engine/debug/DebugOverlay.h"
#include "engine/debug/TraceLog.h"
#include "engine/event/EventData.h"
#include "engine/event/EventDirector.h"
#include "engine/world/World.h"

#include <string>
#include <vector>

namespace resonance {

class DemoScene {
public:
    explicit DemoScene(DemoContentBundle bundle);
    explicit DemoScene(std::vector<RegionData> regions);

    void set_player_position(WorldPosition position);
    void move_player(WorldPosition delta);
    void update();

    std::string current_region_id() const;
    std::string current_music_state() const;
    std::string current_event_id() const;
    std::vector<std::string> overlay_lines() const;
    std::string debug_summary() const;
    WorldPosition player_position() const;
    std::size_t region_count() const;
    const std::vector<RegionData>& regions() const;

private:
    World world_;
    AudioDirector audio_director_;
    EventDirector event_director_;
    DebugOverlay debug_overlay_;
    TraceLog trace_log_{8U};
    std::string current_event_id_;
    std::vector<std::string> overlay_lines_;
};

}  // namespace resonance
