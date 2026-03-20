#include "engine/editor/WorldWorkspaceState.h"

#include "engine/world/RegionData.h"

#include <cmath>
#include <iomanip>
#include <sstream>

namespace resonance {
namespace {

const RegionData* find_region_by_id(const EditorDocument& document, const std::string& id) {
    for (const auto& region : document.regions) {
        if (region.id == id) {
            return &region;
        }
    }
    return nullptr;
}

std::string format_scalar(float value) {
    const float rounded = std::round(value);
    if (std::fabs(value - rounded) < 0.0001F) {
        return std::to_string(static_cast<int>(rounded));
    }

    std::string text = std::to_string(value);
    while (!text.empty() && text.back() == '0') {
        text.pop_back();
    }
    if (!text.empty() && text.back() == '.') {
        text.pop_back();
    }
    return text;
}

std::string format_focus(float value) {
    std::ostringstream stream;
    stream << std::fixed << std::setprecision(2) << value;
    return stream.str();
}

std::string format_layers(const std::vector<std::string>& layers) {
    if (layers.empty()) {
        return "none";
    }

    std::ostringstream stream;
    for (std::size_t index = 0; index < layers.size(); ++index) {
        if (index > 0U) {
            stream << ", ";
        }
        stream << layers[index];
    }
    return stream.str();
}

std::string format_bounds(const RegionBounds& bounds) {
    return format_scalar(bounds.x) + ", " + format_scalar(bounds.y) + ", " +
        format_scalar(bounds.width) + ", " + format_scalar(bounds.height);
}

std::string format_position(float x, float y) {
    return format_scalar(x) + ", " + format_scalar(y);
}

std::string display_or_none(const std::string& value) {
    return value.empty() ? "none" : value;
}

std::string selection_summary_text(const EditorSelection& selection) {
    if (selection.kind == EditorSelectionKind::region && !selection.id.empty()) {
        return "Region: " + selection.id;
    }
    if (selection.kind == EditorSelectionKind::story_anchor && !selection.id.empty()) {
        return "Story Anchor: " + selection.id;
    }
    return "No selection";
}

std::string selection_kind_label(const EditorSelectionKind kind) {
    switch (kind) {
    case EditorSelectionKind::region:
        return "Region";
    case EditorSelectionKind::story_anchor:
        return "Story Anchor";
    case EditorSelectionKind::none:
    default:
        return "None";
    }
}

std::string anchor_count_text(const EditorDocument& document, const std::string& region_id) {
    std::size_t count = 0U;
    for (const auto& anchor : document.story_anchors) {
        if (anchor.region_id == region_id) {
            ++count;
        }
    }
    return std::to_string(count);
}

}  // namespace

std::vector<WorldWorkspaceSection> build_world_workspace_inspector_sections(const EditorInspectorState& inspector_state) {
    std::vector<WorldWorkspaceSection> sections;
    sections.push_back(WorldWorkspaceSection{
        std::string{kWorldWorkspaceSectionSelection},
        "Selection",
        {
            WorldWorkspaceField{"Kind", selection_kind_label(inspector_state.selection.kind)},
            WorldWorkspaceField{"Id", display_or_none(inspector_state.selection.id)},
        },
    });

    if (inspector_state.region.has_value()) {
        const auto& region = *inspector_state.region;
        sections.push_back(WorldWorkspaceSection{
            std::string{kWorldWorkspaceSectionRegion},
            "Region",
            {
                WorldWorkspaceField{"Region", region.id},
                WorldWorkspaceField{"Bounds", format_bounds(RegionBounds{region.x, region.y, region.width, region.height})},
            },
        });
    }

    if (inspector_state.story_anchor.has_value()) {
        const auto& anchor = *inspector_state.story_anchor;
        sections.push_back(WorldWorkspaceSection{
            std::string{kWorldWorkspaceSectionStoryAnchor},
            "Story Anchor",
            {
                WorldWorkspaceField{"Region", anchor.region_id},
                WorldWorkspaceField{"Position", format_position(anchor.x, anchor.y)},
                WorldWorkspaceField{"Activation Radius", format_scalar(anchor.activation_radius)},
                WorldWorkspaceField{"Prompt", display_or_none(anchor.prompt_text)},
                WorldWorkspaceField{"Story", display_or_none(anchor.story_text)},
            },
        });
    }

    return sections;
}

WorldWorkspaceState build_world_workspace_state(
    const EditorDocument& document,
    const EditorSelection& selection,
    const WorldWorkspaceRuntimeSummary& runtime_summary
) {
    const auto inspector_state = build_editor_inspector_state(document, selection);

    std::string context_region_id = runtime_summary.current_region_id;
    if (inspector_state.region.has_value()) {
        context_region_id = inspector_state.region->id;
    } else if (inspector_state.story_anchor.has_value()) {
        context_region_id = inspector_state.story_anchor->region_id;
    }

    const auto* context_region = context_region_id.empty() ? nullptr : find_region_by_id(document, context_region_id);

    WorldWorkspaceState state;
    state.active_workspace_id = display_or_none(runtime_summary.active_workspace_id);
    state.mode_label = display_or_none(runtime_summary.mode_label);
    state.selection_summary = selection_summary_text(selection);
    state.hovered_region_id = runtime_summary.hovered_region_id;
    state.hovered_story_anchor_id = runtime_summary.hovered_story_anchor_id;
    state.drag_active = runtime_summary.drag_active;
    state.hovered_gizmo_id = runtime_summary.hovered_gizmo_id;
    state.drag_delta_x = runtime_summary.drag_delta_x;
    state.drag_delta_y = runtime_summary.drag_delta_y;
    state.active_gizmo_id = runtime_summary.active_gizmo_id;
    state.viewport_origin_x = runtime_summary.viewport_origin_x;
    state.viewport_origin_y = runtime_summary.viewport_origin_y;
    state.viewport_zoom = runtime_summary.viewport_zoom;
    state.viewport_pan_active = runtime_summary.viewport_pan_active;
    state.primary_cards.push_back(WorldWorkspaceCard{
        std::string{kWorldWorkspaceCardInspector},
        "Inspector",
        build_world_workspace_inspector_sections(inspector_state),
    });
    state.primary_cards.push_back(WorldWorkspaceCard{
        std::string{kWorldWorkspaceCardTelemetry},
        "Runtime Telemetry",
        {
            WorldWorkspaceSection{
                std::string{kWorldWorkspaceSectionRuntimeTelemetry},
                "Runtime Telemetry",
                {
                    WorldWorkspaceField{"Current Region", display_or_none(runtime_summary.current_region_id)},
                    WorldWorkspaceField{"Current Music", display_or_none(runtime_summary.current_music_state)},
                    WorldWorkspaceField{"Current Event", display_or_none(runtime_summary.current_event_id)},
                    WorldWorkspaceField{"Story Focus", format_focus(runtime_summary.story_focus)},
                    WorldWorkspaceField{"Event Emphasis", format_focus(runtime_summary.event_emphasis)},
                },
            },
        },
    });
    state.bottom_cards.push_back(WorldWorkspaceCard{
        std::string{kWorldWorkspaceCardRegion},
        "Region Summary",
        {
            WorldWorkspaceSection{
                std::string{kWorldWorkspaceSectionRegionIdentity},
                "Region Identity",
                {
                    WorldWorkspaceField{"Region", context_region != nullptr ? context_region->id : "none"},
                    WorldWorkspaceField{"Bounds", context_region != nullptr ? format_bounds(context_region->bounds) : "none"},
                    WorldWorkspaceField{"Anchor Count", context_region != nullptr ? anchor_count_text(document, context_region->id) : "0"},
                },
            },
        },
    });
    state.bottom_cards.push_back(WorldWorkspaceCard{
        std::string{kWorldWorkspaceCardAudio},
        "Audio Defaults",
        {
            WorldWorkspaceSection{
                std::string{kWorldWorkspaceSectionAudioDefaults},
                "Audio Defaults",
                {
                    WorldWorkspaceField{"Default Music", context_region != nullptr ? context_region->default_music_state : "none"},
                    WorldWorkspaceField{"Ambient Layers", context_region != nullptr ? format_layers(context_region->ambient_layers) : "none"},
                },
            },
        },
    });
    state.bottom_cards.push_back(WorldWorkspaceCard{
        std::string{kWorldWorkspaceCardStatus},
        "Save Status",
        {
            WorldWorkspaceSection{
                std::string{kWorldWorkspaceSectionSaveStatus},
                "Save Status",
                {
                    WorldWorkspaceField{"Dirty", runtime_summary.dirty ? "dirty" : "clean"},
                    WorldWorkspaceField{"Save", display_or_none(runtime_summary.save_status)},
                },
            },
        },
    });
    return state;
}

}  // namespace resonance
