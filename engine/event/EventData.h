#pragma once

#include "engine/audio/MusicStateData.h"
#include "engine/world/RegionData.h"

#include <filesystem>
#include <string>
#include <vector>

namespace resonance {

struct EventData {
    std::string id;
    std::string region_id;
    std::string requested_music_state;
    int weight = 0;
};

struct DemoContentBundle {
    std::vector<RegionData> regions;
    std::vector<MusicStateData> music_states;
    std::vector<EventData> events;
};

std::vector<EventData> load_events(const std::filesystem::path& path);
DemoContentBundle load_demo_content(const std::filesystem::path& root_path);

}  // namespace resonance
