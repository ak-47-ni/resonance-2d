#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace resonance {

struct MusicStateData {
    std::string id;
    std::string bgm_track;
    std::vector<std::string> ambient_layers;
};

std::vector<MusicStateData> load_music_states(const std::filesystem::path& path);

}  // namespace resonance
