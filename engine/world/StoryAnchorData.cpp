#include "engine/world/StoryAnchorData.h"

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

float read_required_float(const nlohmann::json& json, const char* key) {
    if (!json.contains(key) || !json.at(key).is_number()) {
        throw std::runtime_error(std::string("Missing numeric field: ") + key);
    }
    return json.at(key).get<float>();
}

}  // namespace

std::vector<StoryAnchorData> load_story_anchors(const std::filesystem::path& path) {
    const auto root = nlohmann::json::parse(read_text_file(path));
    if (!root.contains("story_anchors") || !root.at("story_anchors").is_array()) {
        throw std::runtime_error("story_anchors.json must contain a 'story_anchors' array");
    }

    std::vector<StoryAnchorData> story_anchors;
    for (const auto& item : root.at("story_anchors")) {
        StoryAnchorData anchor;
        anchor.id = read_required_string(item, "id");
        anchor.region_id = read_required_string(item, "region_id");
        anchor.position.x = read_required_float(item, "x");
        anchor.position.y = read_required_float(item, "y");
        anchor.activation_radius = read_required_float(item, "activation_radius");
        anchor.prompt_text = read_required_string(item, "prompt_text");
        anchor.story_text = read_required_string(item, "story_text");

        if (anchor.activation_radius <= 0.0F) {
            throw std::runtime_error("Story anchor activation_radius must be positive");
        }

        story_anchors.push_back(std::move(anchor));
    }

    return story_anchors;
}

}  // namespace resonance
