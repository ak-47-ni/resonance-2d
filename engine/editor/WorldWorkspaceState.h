#pragma once

#include "engine/editor/EditorDocument.h"

#include <string>
#include <vector>

namespace resonance {

inline constexpr std::string_view kWorldWorkspaceCardInspector = "inspector";
inline constexpr std::string_view kWorldWorkspaceCardTelemetry = "telemetry";
inline constexpr std::string_view kWorldWorkspaceCardRegion = "region";
inline constexpr std::string_view kWorldWorkspaceCardAudio = "audio";
inline constexpr std::string_view kWorldWorkspaceCardStatus = "status";

inline constexpr std::string_view kWorldWorkspaceSectionSelection = "selection";
inline constexpr std::string_view kWorldWorkspaceSectionRegion = "region";
inline constexpr std::string_view kWorldWorkspaceSectionStoryAnchor = "story_anchor";
inline constexpr std::string_view kWorldWorkspaceSectionRuntimeTelemetry = "runtime_telemetry";
inline constexpr std::string_view kWorldWorkspaceSectionRegionIdentity = "region_identity";
inline constexpr std::string_view kWorldWorkspaceSectionAudioDefaults = "audio_defaults";
inline constexpr std::string_view kWorldWorkspaceSectionSaveStatus = "save_status";

struct WorldWorkspaceField {
    std::string label;
    std::string value;
};

struct WorldWorkspaceSection {
    std::string id;
    std::string title;
    std::vector<WorldWorkspaceField> fields;
};

struct WorldWorkspaceCard {
    std::string id;
    std::string title;
    std::vector<WorldWorkspaceSection> sections;
};

struct WorldWorkspaceRuntimeSummary {
    std::string active_workspace_id = "world_workspace";
    std::string mode_label = "Edit";
    std::string current_region_id;
    std::string current_music_state;
    std::string current_event_id;
    float story_focus = 0.0F;
    float event_emphasis = 0.0F;
    bool dirty = false;
    std::string save_status;
    std::string hovered_region_id;
    std::string hovered_story_anchor_id;
    bool drag_active = false;
    std::string hovered_gizmo_id;
    float drag_delta_x = 0.0F;
    float drag_delta_y = 0.0F;
    std::string active_gizmo_id;
    float viewport_origin_x = 0.0F;
    float viewport_origin_y = 0.0F;
    float viewport_zoom = 1.0F;
    bool viewport_pan_active = false;
};

struct WorldWorkspaceState {
    std::string active_workspace_id = "world_workspace";
    std::string mode_label = "Edit";
    std::string selection_summary = "No selection";
    std::string hovered_region_id;
    std::string hovered_story_anchor_id;
    bool drag_active = false;
    std::string hovered_gizmo_id;
    float drag_delta_x = 0.0F;
    float drag_delta_y = 0.0F;
    std::string active_gizmo_id;
    float viewport_origin_x = 0.0F;
    float viewport_origin_y = 0.0F;
    float viewport_zoom = 1.0F;
    bool viewport_pan_active = false;
    std::vector<WorldWorkspaceCard> primary_cards;
    std::vector<WorldWorkspaceCard> bottom_cards;
};

std::vector<WorldWorkspaceSection> build_world_workspace_inspector_sections(const EditorInspectorState& inspector_state);
WorldWorkspaceState build_world_workspace_state(
    const EditorDocument& document,
    const EditorSelection& selection,
    const WorldWorkspaceRuntimeSummary& runtime_summary = {}
);

}  // namespace resonance
