#include "engine/event/EventData.h"

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

int read_required_int(const nlohmann::json& json, const char* key) {
    if (!json.contains(key) || !json.at(key).is_number_integer()) {
        throw std::runtime_error(std::string("Missing integer field: ") + key);
    }
    return json.at(key).get<int>();
}

std::vector<std::string> read_string_array(const nlohmann::json& json, const char* key) {
    if (!json.contains(key)) {
        return {};
    }
    return json.at(key).get<std::vector<std::string>>();
}

float read_optional_mix_scalar(const nlohmann::json& json, const char* key, float fallback) {
    if (!json.contains(key)) {
        return fallback;
    }
    if (!json.at(key).is_number()) {
        throw std::runtime_error(std::string("mix_profile field must be numeric: ") + key);
    }
    return json.at(key).get<float>();
}

EventMixProfile read_mix_profile(const nlohmann::json& json) {
    if (!json.contains("mix_profile")) {
        return {};
    }
    if (!json.at("mix_profile").is_object()) {
        throw std::runtime_error("mix_profile must be an object");
    }

    const auto& mix_profile = json.at("mix_profile");
    return EventMixProfile{
        read_optional_mix_scalar(mix_profile, "event_duck", 1.0F),
        read_optional_mix_scalar(mix_profile, "ambient_boost", 1.0F),
    };
}

}  // namespace

std::vector<EventData> load_events(const std::filesystem::path& path) {
    const auto root = nlohmann::json::parse(read_text_file(path));
    if (!root.contains("events") || !root.at("events").is_array()) {
        throw std::runtime_error("events.json must contain an 'events' array");
    }

    std::vector<EventData> events;
    for (const auto& item : root.at("events")) {
        EventData event;
        event.id = read_required_string(item, "id");
        event.region_id = read_required_string(item, "region_id");
        event.requested_music_state = read_required_string(item, "requested_music_state");
        event.weight = read_required_int(item, "weight");
        event.required_world_tags = read_string_array(item, "required_world_tags");
        event.mix_profile = read_mix_profile(item);
        events.push_back(std::move(event));
    }
    return events;
}

DemoContentBundle load_demo_content(const std::filesystem::path& root_path) {
    DemoContentBundle bundle;
    bundle.regions = load_regions(root_path / "regions" / "regions.json");
    bundle.music_states = load_music_states(root_path / "music" / "music_states.json");
    bundle.events = load_events(root_path / "events" / "events.json");
    bundle.story_anchors = load_story_anchors(root_path / "story" / "story_anchors.json");
    return bundle;
}

}  // namespace resonance
