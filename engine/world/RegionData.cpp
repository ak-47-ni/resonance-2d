#include "engine/world/RegionData.h"

#include "engine/core/FileSystem.h"

#include <nlohmann/json.hpp>

#include <stdexcept>
#include <utility>

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

float read_required_float(const nlohmann::json& json, const char* key) {
    if (!json.contains(key) || !json.at(key).is_number()) {
        throw std::runtime_error(std::string("Missing numeric field: ") + key);
    }
    return json.at(key).get<float>();
}

RegionBounds read_bounds(const nlohmann::json& json) {
    RegionBounds bounds;
    bounds.x = read_required_float(json, "x");
    bounds.y = read_required_float(json, "y");
    bounds.width = read_required_float(json, "width");
    bounds.height = read_required_float(json, "height");

    if (bounds.width <= 0.0F || bounds.height <= 0.0F) {
        throw std::runtime_error("Region bounds must have positive width and height");
    }

    return bounds;
}

}  // namespace

std::vector<RegionData> load_regions(const std::filesystem::path& path) {
    const auto root = nlohmann::json::parse(read_text_file(path));
    if (!root.contains("regions") || !root.at("regions").is_array()) {
        throw std::runtime_error("regions.json must contain a 'regions' array");
    }

    std::vector<RegionData> regions;
    for (const auto& item : root.at("regions")) {
        RegionData region;
        region.id = read_required_string(item, "id");
        region.default_music_state = read_required_string(item, "default_music_state");
        region.ambient_layers = read_string_array(item, "ambient_layers");
        region.bounds = read_bounds(item);
        regions.push_back(std::move(region));
    }
    return regions;
}

}  // namespace resonance
