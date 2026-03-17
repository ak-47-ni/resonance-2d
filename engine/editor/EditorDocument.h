#pragma once

#include "engine/world/RegionData.h"
#include "engine/world/StoryAnchorData.h"

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace resonance {

enum class EditorSelectionKind {
    none,
    region,
    story_anchor,
};

struct EditorSelection {
    EditorSelectionKind kind = EditorSelectionKind::none;
    std::string id;
};

struct StoryAnchorInspectorModel {
    std::string id;
    std::string region_id;
    float x = 0.0F;
    float y = 0.0F;
    float activation_radius = 0.0F;
    std::string prompt_text;
    std::string story_text;
};

struct RegionInspectorModel {
    std::string id;
    std::string default_music_state;
    std::vector<std::string> ambient_layers;
    float x = 0.0F;
    float y = 0.0F;
    float width = 0.0F;
    float height = 0.0F;
};

struct EditorInspectorState {
    EditorSelection selection;
    std::optional<StoryAnchorInspectorModel> story_anchor;
    std::optional<RegionInspectorModel> region;
};

struct EditorDocument {
    std::vector<RegionData> regions;
    std::vector<StoryAnchorData> story_anchors;
};

EditorDocument load_editor_document(const std::filesystem::path& data_root);
EditorInspectorState build_editor_inspector_state(const EditorDocument& document, const EditorSelection& selection);
bool apply_story_anchor_inspector(EditorDocument& document, const StoryAnchorInspectorModel& inspector);
bool apply_region_inspector(EditorDocument& document, const RegionInspectorModel& inspector);
void write_editor_document(const EditorDocument& document, const std::filesystem::path& data_root);

}  // namespace resonance
