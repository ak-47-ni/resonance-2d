#include "engine/editor/EditorDocument.h"

#include "engine/world/RegionData.h"
#include "engine/world/StoryAnchorData.h"

#include <nlohmann/json.hpp>

#include <cmath>
#include <fstream>
#include <stdexcept>

namespace resonance {
namespace {

using ordered_json = nlohmann::ordered_json;

ordered_json json_number(float value) {
    const float rounded = std::round(value);
    if (std::fabs(value - rounded) < 0.0001F) {
        return static_cast<int>(rounded);
    }
    return value;
}

void write_json_file(const std::filesystem::path& path, const ordered_json& json) {
    std::filesystem::create_directories(path.parent_path());
    std::ofstream stream(path);
    if (!stream.is_open()) {
        throw std::runtime_error("Failed to open file for write: " + path.string());
    }
    stream << json.dump(2) << '\n';
}

void validate_story_anchor(const StoryAnchorData& anchor) {
    if (anchor.id.empty()) {
        throw std::runtime_error("Story anchor id must not be empty");
    }
    if (anchor.region_id.empty()) {
        throw std::runtime_error("Story anchor region_id must not be empty");
    }
    if (anchor.activation_radius <= 0.0F) {
        throw std::runtime_error("Story anchor activation_radius must be positive");
    }
}

void validate_region(const RegionData& region) {
    if (region.id.empty()) {
        throw std::runtime_error("Region id must not be empty");
    }
    if (region.default_music_state.empty()) {
        throw std::runtime_error("Region default_music_state must not be empty");
    }
    if (region.bounds.width <= 0.0F || region.bounds.height <= 0.0F) {
        throw std::runtime_error("Region bounds must have positive width and height");
    }
}

ordered_json serialize_story_anchor(const StoryAnchorData& anchor) {
    validate_story_anchor(anchor);
    ordered_json item = ordered_json::object();
    item["id"] = anchor.id;
    item["region_id"] = anchor.region_id;
    item["x"] = json_number(anchor.position.x);
    item["y"] = json_number(anchor.position.y);
    item["activation_radius"] = json_number(anchor.activation_radius);
    item["prompt_text"] = anchor.prompt_text;
    item["story_text"] = anchor.story_text;
    return item;
}

ordered_json serialize_region(const RegionData& region) {
    validate_region(region);
    ordered_json item = ordered_json::object();
    item["id"] = region.id;
    item["default_music_state"] = region.default_music_state;
    item["ambient_layers"] = region.ambient_layers;
    item["x"] = json_number(region.bounds.x);
    item["y"] = json_number(region.bounds.y);
    item["width"] = json_number(region.bounds.width);
    item["height"] = json_number(region.bounds.height);
    return item;
}

}  // namespace

EditorDocument load_editor_document(const std::filesystem::path& data_root) {
    return EditorDocument{
        load_regions(data_root / "regions" / "regions.json"),
        load_story_anchors(data_root / "story" / "story_anchors.json"),
    };
}

EditorInspectorState build_editor_inspector_state(const EditorDocument& document, const EditorSelection& selection) {
    EditorInspectorState state;
    state.selection = selection;

    if (selection.kind == EditorSelectionKind::story_anchor) {
        for (const auto& anchor : document.story_anchors) {
            if (anchor.id != selection.id) {
                continue;
            }
            state.story_anchor = StoryAnchorInspectorModel{
                anchor.id,
                anchor.region_id,
                anchor.position.x,
                anchor.position.y,
                anchor.activation_radius,
                anchor.prompt_text,
                anchor.story_text,
            };
            break;
        }
    }

    if (selection.kind == EditorSelectionKind::region) {
        for (const auto& region : document.regions) {
            if (region.id != selection.id) {
                continue;
            }
            state.region = RegionInspectorModel{
                region.id,
                region.default_music_state,
                region.ambient_layers,
                region.bounds.x,
                region.bounds.y,
                region.bounds.width,
                region.bounds.height,
            };
            break;
        }
    }

    return state;
}

bool apply_story_anchor_inspector(EditorDocument& document, const StoryAnchorInspectorModel& inspector) {
    for (auto& anchor : document.story_anchors) {
        if (anchor.id != inspector.id) {
            continue;
        }
        anchor.region_id = inspector.region_id;
        anchor.position.x = inspector.x;
        anchor.position.y = inspector.y;
        anchor.activation_radius = inspector.activation_radius;
        anchor.prompt_text = inspector.prompt_text;
        anchor.story_text = inspector.story_text;
        validate_story_anchor(anchor);
        return true;
    }
    return false;
}

bool apply_region_inspector(EditorDocument& document, const RegionInspectorModel& inspector) {
    for (auto& region : document.regions) {
        if (region.id != inspector.id) {
            continue;
        }
        region.default_music_state = inspector.default_music_state;
        region.ambient_layers = inspector.ambient_layers;
        region.bounds.x = inspector.x;
        region.bounds.y = inspector.y;
        region.bounds.width = inspector.width;
        region.bounds.height = inspector.height;
        validate_region(region);
        return true;
    }
    return false;
}

void write_editor_document(const EditorDocument& document, const std::filesystem::path& data_root) {
    ordered_json region_root = ordered_json::object();
    region_root["regions"] = ordered_json::array();
    for (const auto& region : document.regions) {
        region_root["regions"].push_back(serialize_region(region));
    }

    ordered_json story_root = ordered_json::object();
    story_root["story_anchors"] = ordered_json::array();
    for (const auto& anchor : document.story_anchors) {
        story_root["story_anchors"].push_back(serialize_story_anchor(anchor));
    }

    write_json_file(data_root / "regions" / "regions.json", region_root);
    write_json_file(data_root / "story" / "story_anchors.json", story_root);
}

}  // namespace resonance
