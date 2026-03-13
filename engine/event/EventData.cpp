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
        events.push_back(std::move(event));
    }
    return events;
}

DemoContentBundle load_demo_content(const std::filesystem::path& root_path) {
    DemoContentBundle bundle;
    bundle.regions = load_regions(root_path / "regions" / "regions.json");
    bundle.music_states = load_music_states(root_path / "music" / "music_states.json");
    bundle.events = load_events(root_path / "events" / "events.json");
    return bundle;
}

}  // namespace resonance
