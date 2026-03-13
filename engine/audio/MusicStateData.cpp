#include "engine/audio/MusicStateData.h"

#include "engine/core/FileSystem.h"

#include <nlohmann/json.hpp>

#include <stdexcept>

namespace resonance {
namespace {

std::string read_required_string(const nlohmann::json& json, const char* key) {
    if (!json.contains(key) || !json.at(key).is_string()) {
        throw std::runtime_error(std::string("Missing string field: ") + key);
    }
    return json.at(key).get<std::string>();
}

std::vector<std::string> read_string_array(const nlohmann::json& json, const char* key) {
    if (!json.contains(key)) {
        return {};
    }
    return json.at(key).get<std::vector<std::string>>();
}

}  // namespace

std::vector<MusicStateData> load_music_states(const std::filesystem::path& path) {
    const auto root = nlohmann::json::parse(read_text_file(path));
    if (!root.contains("music_states") || !root.at("music_states").is_array()) {
        throw std::runtime_error("music_states.json must contain a 'music_states' array");
    }

    std::vector<MusicStateData> music_states;
    for (const auto& item : root.at("music_states")) {
        MusicStateData music_state;
        music_state.id = read_required_string(item, "id");
        music_state.bgm_track = read_required_string(item, "bgm_track");
        music_state.ambient_layers = read_string_array(item, "ambient_layers");
        music_states.push_back(std::move(music_state));
    }
    return music_states;
}

}  // namespace resonance
