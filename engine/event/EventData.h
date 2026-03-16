#pragma once

#include "engine/audio/MusicStateData.h"
#include "engine/world/RegionData.h"
#include "engine/world/StoryAnchorData.h"

#include <filesystem>
#include <string>
#include <vector>

namespace resonance {

struct EventMixProfile {
    float event_duck = 1.0F;
    float ambient_boost = 1.0F;
};

struct EventData {
    std::string id;
    std::string region_id;
    std::string requested_music_state;
    int weight = 0;
    std::vector<std::string> required_world_tags;
    EventMixProfile mix_profile;
};

struct DemoContentBundle {
    std::vector<RegionData> regions;
    std::vector<MusicStateData> music_states;
    std::vector<EventData> events;
    std::vector<StoryAnchorData> story_anchors;
};

std::vector<EventData> load_events(const std::filesystem::path& path);
DemoContentBundle load_demo_content(const std::filesystem::path& root_path);

}  // namespace resonance
