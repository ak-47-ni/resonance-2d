#pragma once

#include "engine/world/Trigger.h"

#include <filesystem>
#include <string>
#include <vector>

namespace resonance {

struct StoryAnchorData {
    std::string id;
    std::string region_id;
    WorldPosition position;
    float activation_radius = 0.0F;
    std::string prompt_text;
    std::string story_text;
};

std::vector<StoryAnchorData> load_story_anchors(const std::filesystem::path& path);

}  // namespace resonance
