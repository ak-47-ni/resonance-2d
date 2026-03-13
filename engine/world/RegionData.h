#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace resonance {

struct RegionBounds {
    float x = 0.0F;
    float y = 0.0F;
    float width = 0.0F;
    float height = 0.0F;
};

struct RegionData {
    std::string id;
    std::string default_music_state;
    std::vector<std::string> ambient_layers;
    RegionBounds bounds;
};

std::vector<RegionData> load_regions(const std::filesystem::path& path);

}  // namespace resonance
