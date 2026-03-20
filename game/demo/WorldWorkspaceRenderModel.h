#pragma once

#include "engine/editor/WorldWorkspaceState.h"
#include "game/demo/WorldWorkspaceViewModel.h"

#include <array>
#include <cmath>
#include <string>
#include <string_view>
#include <vector>

namespace resonance {

struct WorkspaceRenderCard {
    std::string title;
    std::vector<std::string> lines;
};

struct WorkspaceRailItemRender {
    std::string label;
    bool active = false;
};

struct WorkspaceTabRender {
    std::string label;
    bool active = false;
};

struct WorldWorkspaceRenderModel {
    std::string header_subtitle;
    std::string workspace_tone;
    std::string rail_label;
    std::string page_title;
    std::string page_subtitle;
    std::array<std::string, 3> page_context_badges{};
    std::string shell_footer;
    std::array<WorkspaceTabRender, 4> header_tabs{};
    std::array<std::string, 3> header_status_badges{};
    std::array<std::string, 2> properties_footer_badges{};
    std::array<std::string, 2> properties_footer_lines{};
    std::array<WorkspaceRailItemRender, 4> rail_items{};
    std::array<std::string, 2> action_badges{};
    std::string viewport_heading;
    std::string viewport_badge;
    std::string viewport_frame_state;
    std::string viewport_frame_detail;
    std::array<std::string, 3> viewport_meta_badges{};
    std::string viewport_focus_label;
    std::string viewport_focus_detail;
    std::array<std::string, 2> viewport_hint_lines{};
    std::string viewport_gizmo_label;
    std::string viewport_gizmo_detail;
    std::string viewport_ghost_label;
    std::string viewport_ghost_detail;
    std::string inspector_focus_label;
    std::string inspector_focus_detail;
    std::array<std::string, 2> inspector_focus_badges{};
    int inspector_emphasis_index = 0;
    std::string toolbar_prefix;
    std::string toolbar_selection;
    std::string toolbar_status;
    std::array<std::string, 3> toolbar_context_badges{};
    std::string toolbar_tool_mode;
    std::string toolbar_tool_cursor;
    std::array<WorkspaceRenderCard, 3> bottom_cards{};
    std::array<WorkspaceRenderCard, 3> inspector_cards{};
};

inline std::string world_workspace_slug(std::string text) {
    text = world_workspace_lower_ascii(std::move(text));
    for (char& ch : text) {
        if (ch == ' ' || ch == '-' || ch == '/') {
            ch = '_';
        }
    }
    return text;
}

inline std::string world_workspace_format_scalar(float value) {
    const int rounded = static_cast<int>(value);
    if (value == static_cast<float>(rounded)) {
        return std::to_string(rounded);
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


inline std::string world_workspace_format_signed_scalar(float value) {
    const std::string magnitude = world_workspace_format_scalar(std::fabs(value));
    return (value >= 0.0F ? "+" : "-") + magnitude;
}

inline std::string world_workspace_format_zoom(float value) {
    if (std::fabs(value - 1.0F) < 0.0001F) {
        return "1.00x";
    }
    const int hundredths = static_cast<int>(std::round(value * 100.0F));
    const int integral = hundredths / 100;
    const int fractional = std::abs(hundredths % 100);
    std::string text = std::to_string(integral) + ".";
    if (fractional < 10) {
        text += "0";
    }
    text += std::to_string(fractional) + "x";
    return text;
}

inline bool world_workspace_state_starts_with(const WorldWorkspaceState& state, std::string_view prefix);

inline std::string world_workspace_active_gizmo_id(const WorldWorkspaceState& state) {
    return state.active_gizmo_id.empty() ? state.hovered_gizmo_id : state.active_gizmo_id;
}


inline std::string build_world_workspace_tool_mode(const WorldWorkspaceState& state) {
    const std::string gizmo_id = world_workspace_active_gizmo_id(state);
    if (state.viewport_pan_active) {
        return "tool:navigate_view";
    }
    if (state.drag_active && gizmo_id == "region_resize") {
        return "tool:resize_bounds";
    }
    if (state.drag_active && gizmo_id == "anchor_radius") {
        return "tool:adjust_radius";
    }
    if (state.drag_active && world_workspace_state_starts_with(state, "Region:")) {
        return "tool:move_region";
    }
    if (state.drag_active && world_workspace_state_starts_with(state, "Story Anchor:")) {
        return "tool:move_anchor";
    }
    if (gizmo_id == "region_resize") {
        return "tool:resize_bounds";
    }
    if (gizmo_id == "anchor_radius") {
        return "tool:adjust_radius";
    }
    if (world_workspace_state_starts_with(state, "Region:")) {
        return "tool:move_region";
    }
    if (world_workspace_state_starts_with(state, "Story Anchor:")) {
        return "tool:move_anchor";
    }
    return "tool:select";
}

inline std::string build_world_workspace_tool_cursor(const WorldWorkspaceState& state) {
    const std::string gizmo_id = world_workspace_active_gizmo_id(state);
    if (state.viewport_pan_active) {
        return "cursor:grabbing";
    }
    if (state.drag_active && (gizmo_id == "region_resize" || gizmo_id == "anchor_radius")) {
        return gizmo_id == "region_resize" ? "cursor:nwse-resize" : "cursor:ew-resize";
    }
    if (state.drag_active && (!state.hovered_region_id.empty() || !state.hovered_story_anchor_id.empty() ||
                              world_workspace_state_starts_with(state, "Region:") ||
                              world_workspace_state_starts_with(state, "Story Anchor:"))) {
        return "cursor:grabbing";
    }
    if (gizmo_id == "region_resize") {
        return "cursor:nwse-resize";
    }
    if (gizmo_id == "anchor_radius") {
        return "cursor:ew-resize";
    }
    if (!state.hovered_region_id.empty() || !state.hovered_story_anchor_id.empty()) {
        return "cursor:pointer";
    }
    if (world_workspace_state_starts_with(state, "Region:") || world_workspace_state_starts_with(state, "Story Anchor:")) {
        return "cursor:move";
    }
    return "cursor:crosshair";
}

inline WorkspaceRenderCard build_world_workspace_canvas_tools_card(const WorldWorkspaceState& state) {
    const std::string tool_mode = build_world_workspace_tool_mode(state);
    const std::string gizmo_id = world_workspace_active_gizmo_id(state);
    const bool move_bounds_active = state.drag_active && gizmo_id == "region_resize";
    const bool move_bounds_hot = !move_bounds_active && gizmo_id == "region_resize";
    std::string view_scope = "viewport_only";
    if (state.viewport_pan_active) {
        view_scope = "navigate_view";
    } else if (state.drag_active || world_workspace_state_starts_with(state, "Region:") ||
               world_workspace_state_starts_with(state, "Story Anchor:")) {
        view_scope = "selection_focus";
    } else if (!state.hovered_region_id.empty() || !state.hovered_story_anchor_id.empty() || !state.hovered_gizmo_id.empty()) {
        view_scope = "hover_focus";
    }
    WorkspaceRenderCard render_card;
    render_card.title = "canvas_tools";
    render_card.lines = {
        std::string{"tool_select: "} + (tool_mode == "tool:select" ? "active" : "enabled"),
        std::string{"tool_move_bounds: "} + (move_bounds_active ? "active" : (move_bounds_hot ? "hot" : "enabled")),
        std::string{"tool_toggle_anchors: "} + (state.active_workspace_id == "anchor_workspace" ? "active" : "enabled"),
        std::string{"view_zoom: "} + world_workspace_format_zoom(state.viewport_zoom),
        std::string{"view_origin: "} + world_workspace_format_signed_scalar(state.viewport_origin_x) + ", " + world_workspace_format_signed_scalar(state.viewport_origin_y),
        std::string{"view_pan: "} + (state.viewport_pan_active ? "active" : "enabled"),
        std::string{"view_scope: "} + view_scope,
    };
    return render_card;
}

inline bool world_workspace_state_starts_with(const WorldWorkspaceState& state, std::string_view prefix) {
    return state.selection_summary.rfind(prefix.data(), 0) == 0;
}

inline WorkspaceRenderCard build_workspace_render_card(const WorldWorkspaceCard* card, std::size_t max_lines) {
    WorkspaceRenderCard render_card;
    render_card.title = card == nullptr ? "unavailable" : world_workspace_slug(card->title);

    if (card == nullptr) {
        render_card.lines.push_back("No data");
        return render_card;
    }

    for (const auto& section : card->sections) {
        for (const auto& field : section.fields) {
            if (render_card.lines.size() >= max_lines) {
                break;
            }
            render_card.lines.push_back(field.label + std::string{": "} + field.value);
        }
        if (render_card.lines.size() >= max_lines) {
            break;
        }
    }

    if (render_card.lines.empty()) {
        render_card.lines.push_back("No data");
    }
    return render_card;
}

inline WorkspaceRenderCard build_world_workspace_inspector_render_card(const WorldWorkspaceState& state) {
    const auto* inspector_card = world_workspace_find_card(state, kWorldWorkspaceCardInspector);
    const auto* region = world_workspace_find_section(inspector_card, kWorldWorkspaceSectionRegion);
    const auto* story_anchor = world_workspace_find_section(inspector_card, kWorldWorkspaceSectionStoryAnchor);
    const auto* selection = world_workspace_find_section(inspector_card, kWorldWorkspaceSectionSelection);

    WorkspaceRenderCard render_card;
    render_card.title = inspector_card == nullptr ? "inspector" : world_workspace_slug(inspector_card->title);
    render_card.lines.push_back(std::string{"Selection: "} + state.selection_summary);

    if (story_anchor != nullptr) {
        render_card.lines.push_back(world_workspace_find_field(story_anchor, "Region"));
        render_card.lines.push_back(world_workspace_find_field(story_anchor, "Position"));
        render_card.lines.push_back(world_workspace_find_field(story_anchor, "Activation Radius"));
        return render_card;
    }

    if (region != nullptr) {
        render_card.lines.push_back(world_workspace_find_field(region, "Region"));
        render_card.lines.push_back(world_workspace_find_field(region, "Bounds"));
        return render_card;
    }

    if (selection != nullptr) {
        render_card.lines.push_back(world_workspace_find_field(selection, "Kind"));
        render_card.lines.push_back(world_workspace_find_field(selection, "Id"));
    }

    return render_card;
}

inline WorkspaceRenderCard build_world_workspace_session_render_card(const WorldWorkspaceViewModel& view_model) {
    WorkspaceRenderCard render_card;
    render_card.title = "session_status";
    render_card.lines = {
        view_model.session_status_lines[0],
        view_model.session_status_lines[1],
        view_model.selection_summary,
        view_model.save_status_label,
    };
    return render_card;
}

inline WorkspaceRenderCard build_anchor_workspace_selection_card(const WorldWorkspaceState& state) {
    auto render_card = build_world_workspace_inspector_render_card(state);
    render_card.title = "anchor_selection";
    return render_card;
}

inline WorkspaceRenderCard build_anchor_workspace_runtime_card(const WorldWorkspaceState& state) {
    const auto* telemetry_card = world_workspace_find_card(state, kWorldWorkspaceCardTelemetry);
    const auto* telemetry = world_workspace_find_section(telemetry_card, kWorldWorkspaceSectionRuntimeTelemetry);
    WorkspaceRenderCard render_card;
    render_card.title = "runtime_context";
    render_card.lines = {
        world_workspace_find_field(telemetry, "Current Region"),
        world_workspace_find_field(telemetry, "Current Music"),
        world_workspace_find_field(telemetry, "Current Event"),
        world_workspace_find_field(telemetry, "Story Focus"),
    };
    return render_card;
}

inline WorkspaceRenderCard build_anchor_workspace_overview_card(const WorldWorkspaceState& state) {
    const auto* inspector_card = world_workspace_find_card(state, kWorldWorkspaceCardInspector);
    const auto* selection = world_workspace_find_section(inspector_card, kWorldWorkspaceSectionSelection);
    const auto* region_card = world_workspace_find_card(state, kWorldWorkspaceCardRegion);
    const auto* region = world_workspace_find_section(region_card, kWorldWorkspaceSectionRegionIdentity);
    WorkspaceRenderCard render_card;
    render_card.title = "anchor_overview";
    render_card.lines = {
        std::string{"Selection: "} + state.selection_summary,
        world_workspace_find_field(selection, "Kind"),
        world_workspace_find_field(region, "Region"),
        world_workspace_find_field(region, "Anchor Count"),
    };
    return render_card;
}

inline WorkspaceRenderCard build_anchor_workspace_audio_card(const WorldWorkspaceState& state) {
    const auto* audio_card = world_workspace_find_card(state, kWorldWorkspaceCardAudio);
    const auto* audio = world_workspace_find_section(audio_card, kWorldWorkspaceSectionAudioDefaults);
    const auto* telemetry_card = world_workspace_find_card(state, kWorldWorkspaceCardTelemetry);
    const auto* telemetry = world_workspace_find_section(telemetry_card, kWorldWorkspaceSectionRuntimeTelemetry);
    WorkspaceRenderCard render_card;
    render_card.title = "anchor_audio";
    render_card.lines = {
        world_workspace_find_field(audio, "Default Music"),
        world_workspace_find_field(audio, "Ambient Layers"),
        world_workspace_find_field(telemetry, "Current Music"),
        world_workspace_find_field(telemetry, "Current Event"),
    };
    return render_card;
}

inline WorkspaceRenderCard build_anchor_workspace_status_card(const WorldWorkspaceState& state, const WorldWorkspaceViewModel& view_model) {
    const auto* status_card = world_workspace_find_card(state, kWorldWorkspaceCardStatus);
    const auto* status = world_workspace_find_section(status_card, kWorldWorkspaceSectionSaveStatus);
    WorkspaceRenderCard render_card;
    render_card.title = "anchor_status";
    render_card.lines = {
        world_workspace_find_field(status, "Dirty"),
        world_workspace_find_field(status, "Save"),
        view_model.active_workspace_label,
        state.selection_summary,
    };
    return render_card;
}

inline std::array<WorkspaceRailItemRender, 4> build_world_workspace_rail_items(const WorldWorkspaceState& state) {
    const bool world_active = state.active_workspace_id == "world_workspace" || state.active_workspace_id == "none";
    return {
        WorkspaceRailItemRender{"world_region_editor", world_active},
        WorkspaceRailItemRender{"story_anchor_editor", false},
        WorkspaceRailItemRender{"event_authoring", false},
        WorkspaceRailItemRender{"audio_mix_workspace", false},
    };
}

inline std::array<WorkspaceTabRender, 4> build_world_workspace_header_tabs(const WorldWorkspaceState& state) {
    const bool world_active = state.active_workspace_id != "anchor_workspace";
    const bool anchor_active = state.active_workspace_id == "anchor_workspace";
    return {
        WorkspaceTabRender{"World", world_active},
        WorkspaceTabRender{"Anchors", anchor_active},
        WorkspaceTabRender{"Events", false},
        WorkspaceTabRender{"Audio", false},
    };
}

inline std::string build_world_workspace_header_subtitle(const WorldWorkspaceState& state) {
    if (state.active_workspace_id == "anchor_workspace") {
        return "anchor_workspace // shell_focus";
    }
    return "world_workspace // world_focus";
}

inline std::string build_world_workspace_toolbar_scope_badge(const WorldWorkspaceState& state) {
    if (state.viewport_pan_active) {
        return "scope:nav";
    }
    if (state.drag_active) {
        return "scope:preview";
    }
    if (world_workspace_state_starts_with(state, "Region:") || world_workspace_state_starts_with(state, "Story Anchor:")) {
        return "scope:select";
    }
    if (!state.hovered_region_id.empty() || !state.hovered_story_anchor_id.empty() || !state.hovered_gizmo_id.empty()) {
        return "scope:hover";
    }
    return "scope:view";
}

inline std::string build_world_workspace_toolbar_tool_badge(const WorldWorkspaceState& state) {
    const std::string tool_mode = build_world_workspace_tool_mode(state);
    if (tool_mode == "tool:navigate_view") {
        return "tool:navigate";
    }
    if (tool_mode == "tool:resize_bounds") {
        return "tool:resize";
    }
    if (tool_mode == "tool:move_region" || tool_mode == "tool:move_anchor") {
        return "tool:move";
    }
    if (tool_mode == "tool:adjust_radius") {
        return "tool:radius";
    }
    return "tool:select";
}

inline std::string build_world_workspace_toolbar_cursor_badge(const WorldWorkspaceState& state) {
    const std::string cursor = build_world_workspace_tool_cursor(state);
    if (cursor == "cursor:crosshair") {
        return "cursor:xhair";
    }
    if (cursor == "cursor:pointer") {
        return "cursor:pointer";
    }
    if (cursor == "cursor:grabbing") {
        return "cursor:grab";
    }
    if (cursor == "cursor:nwse-resize") {
        return "cursor:nwse";
    }
    if (cursor == "cursor:ew-resize") {
        return "cursor:ew";
    }
    if (cursor == "cursor:move") {
        return "cursor:move";
    }
    return "cursor:default";
}

inline std::array<std::string, 3> build_world_workspace_toolbar_context_badges(const WorldWorkspaceState& state) {
    return {
        build_world_workspace_toolbar_scope_badge(state),
        build_world_workspace_toolbar_tool_badge(state),
        build_world_workspace_toolbar_cursor_badge(state),
    };
}

inline std::array<std::string, 3> build_world_workspace_header_status_badges(const WorldWorkspaceViewModel& view_model) {
    return {
        view_model.save_status_label,
        view_model.dirty_label,
        view_model.mode_label,
    };
}

inline std::array<std::string, 2> build_world_workspace_action_badges(const WorldWorkspaceState& state) {
    std::string primary = "playtest_route";
    if (world_workspace_state_starts_with(state, "Story Anchor:")) {
        primary = "focus_anchor";
    } else if (world_workspace_state_starts_with(state, "Region:")) {
        primary = "preview_region";
    }
    return {"export_layout", primary};
}

inline std::array<std::string, 3> build_world_workspace_page_context_badges(const WorldWorkspaceState& state) {
    std::string workspace_badge = state.active_workspace_id == "anchor_workspace" ? "workspace:anchor" : "workspace:world";
    std::string frame_badge = "frame:view";
    if (state.viewport_pan_active) {
        frame_badge = "frame:nav";
    } else if (state.drag_active) {
        frame_badge = "frame:preview";
    } else if (world_workspace_state_starts_with(state, "Region:") || world_workspace_state_starts_with(state, "Story Anchor:")) {
        frame_badge = "frame:select";
    } else if (!state.hovered_region_id.empty() || !state.hovered_story_anchor_id.empty() || !state.hovered_gizmo_id.empty()) {
        frame_badge = "frame:hover";
    }

    std::string selection_badge = "selection:none";
    if (world_workspace_state_starts_with(state, "Story Anchor:")) {
        selection_badge = "selection:anchor";
    } else if (world_workspace_state_starts_with(state, "Region:")) {
        selection_badge = "selection:region";
    }

    return {workspace_badge, frame_badge, selection_badge};
}

inline std::string build_world_workspace_page_title(const WorldWorkspaceState& state) {
    if (state.active_workspace_id == "anchor_workspace" && state.selection_summary == "No selection") {
        return "ANCHOR WORKSPACE";
    }
    if (world_workspace_state_starts_with(state, "Story Anchor:")) {
        return "STORY ANCHOR // " + state.selection_summary.substr(std::string{"Story Anchor: "}.size());
    }
    if (world_workspace_state_starts_with(state, "Region:")) {
        return "REGION FOCUS // " + state.selection_summary.substr(std::string{"Region: "}.size());
    }
    return "WORLD / REGION WORKSPACE";
}

inline std::string build_world_workspace_page_subtitle(const WorldWorkspaceState& state) {
    if (state.active_workspace_id == "anchor_workspace" && state.selection_summary == "No selection") {
        return "shape story triggers, prompts, and activation zones";
    }
    if (world_workspace_state_starts_with(state, "Story Anchor:")) {
        const auto* inspector_card = world_workspace_find_card(state, kWorldWorkspaceCardInspector);
        const auto* story_anchor = world_workspace_find_section(inspector_card, kWorldWorkspaceSectionStoryAnchor);
        const std::string anchor_id = state.selection_summary.substr(std::string{"Story Anchor: "}.size());
        const std::string region_id = world_workspace_line_value(world_workspace_find_field(story_anchor, "Region"));
        return "anchor focus // " + anchor_id + " in " + region_id;
    }
    if (world_workspace_state_starts_with(state, "Region:")) {
        return "region focus // " + state.selection_summary.substr(std::string{"Region: "}.size());
    }
    return "edit bounds, ambience defaults, region tags, and playable context";
}

inline std::string build_world_workspace_shell_footer(const WorldWorkspaceState& state) {
    if (world_workspace_state_starts_with(state, "Story Anchor:")) {
        return "world_workspace // anchor-focused runtime shell";
    }
    if (world_workspace_state_starts_with(state, "Region:")) {
        return "world_workspace // region-focused runtime shell";
    }
    return "world_workspace // runtime shell aligned to prototype";
}

inline std::array<std::string, 2> build_world_workspace_properties_footer_badges(const WorldWorkspaceState& state) {
    const std::string workspace_badge = state.active_workspace_id == "anchor_workspace" ? "workspace:anchor" : "workspace:world";
    std::string selection_badge = "selection:none";
    if (world_workspace_state_starts_with(state, "Story Anchor:")) {
        selection_badge = "selection:anchor";
    } else if (world_workspace_state_starts_with(state, "Region:")) {
        selection_badge = "selection:region";
    }
    return {workspace_badge, selection_badge};
}

inline std::array<std::string, 2> build_world_workspace_properties_footer_lines(
    const WorldWorkspaceState& state,
    const WorldWorkspaceViewModel& view_model
) {
    if (world_workspace_state_starts_with(state, "Story Anchor:")) {
        return {view_model.active_workspace_label, view_model.save_status_label};
    }
    return {view_model.active_workspace_label, state.selection_summary};
}

inline std::string build_world_workspace_tone(const WorldWorkspaceState& state) {
    if (state.active_workspace_id == "anchor_workspace" || world_workspace_state_starts_with(state, "Story Anchor:")) {
        return "anchor";
    }
    return "world";
}

inline std::string build_world_workspace_viewport_heading(const WorldWorkspaceState& state) {
    if (world_workspace_state_starts_with(state, "Story Anchor:")) {
        return "active_layer // anchor focus + triggers";
    }
    if (world_workspace_state_starts_with(state, "Region:")) {
        return "active_layer // region bounds + anchors";
    }
    return "active_layer // regions + anchors + triggers";
}

inline std::string build_world_workspace_viewport_badge(const WorldWorkspaceState& state) {
    if (state.active_workspace_id == "anchor_workspace") {
        return "Anchor Canvas";
    }
    return "World Canvas";
}

inline std::string build_world_workspace_viewport_frame_state(const WorldWorkspaceState& state) {
    const std::string gizmo_id = world_workspace_active_gizmo_id(state);
    if (state.viewport_pan_active) {
        return "frame:navigate_view";
    }
    if (state.drag_active && (gizmo_id == "anchor_radius" || gizmo_id == "region_resize" ||
                              world_workspace_state_starts_with(state, "Region:") ||
                              world_workspace_state_starts_with(state, "Story Anchor:"))) {
        return "frame:edit_preview";
    }
    if (world_workspace_state_starts_with(state, "Region:") || world_workspace_state_starts_with(state, "Story Anchor:")) {
        return "frame:selection_focus";
    }
    if (!state.hovered_region_id.empty() || !state.hovered_story_anchor_id.empty() || !state.hovered_gizmo_id.empty()) {
        return "frame:hover_focus";
    }
    return "frame:viewport_only";
}

inline std::string build_world_workspace_viewport_frame_detail(const WorldWorkspaceState& state) {
    const std::string gizmo_id = world_workspace_active_gizmo_id(state);
    if (state.viewport_pan_active) {
        return "Viewport framing active";
    }
    if (state.drag_active && (gizmo_id == "anchor_radius" || gizmo_id == "region_resize" ||
                              world_workspace_state_starts_with(state, "Region:") ||
                              world_workspace_state_starts_with(state, "Story Anchor:"))) {
        return "Preview remains clipped to canvas";
    }
    if (world_workspace_state_starts_with(state, "Region:") || world_workspace_state_starts_with(state, "Story Anchor:")) {
        return "Handles active in canvas";
    }
    if (!state.hovered_region_id.empty() || !state.hovered_story_anchor_id.empty() || !state.hovered_gizmo_id.empty()) {
        return "Canvas target hot";
    }
    return "Edit inside canvas bounds";
}

inline std::array<std::string, 3> build_world_workspace_viewport_meta_badges(const WorldWorkspaceState& state) {
    return {
        "zoom:" + world_workspace_format_zoom(state.viewport_zoom),
        "origin:" + world_workspace_format_signed_scalar(state.viewport_origin_x) + "," + world_workspace_format_signed_scalar(state.viewport_origin_y),
        std::string{"pan:"} + (state.viewport_pan_active ? "active" : "idle")};
}

inline std::string build_world_workspace_viewport_focus_label(const WorldWorkspaceState& state) {
    const std::string gizmo_id = world_workspace_active_gizmo_id(state);
    if (state.viewport_pan_active) {
        return "Navigating View";
    }
    if (state.drag_active && gizmo_id == "anchor_radius") {
        return "Adjusting Radius";
    }
    if (state.drag_active && gizmo_id == "region_resize") {
        return "Resizing Region";
    }
    if (state.drag_active && world_workspace_state_starts_with(state, "Story Anchor:")) {
        return "Moving Anchor";
    }
    if (state.drag_active && world_workspace_state_starts_with(state, "Region:")) {
        return "Moving Region";
    }
    if (world_workspace_state_starts_with(state, "Story Anchor:")) {
        return "Anchor Focus";
    }
    if (world_workspace_state_starts_with(state, "Region:")) {
        return "Region Focus";
    }
    if (!state.hovered_story_anchor_id.empty()) {
        return "Hover Anchor";
    }
    if (!state.hovered_region_id.empty()) {
        return "Hover Region";
    }
    if (state.active_workspace_id == "anchor_workspace") {
        return "Anchor Sweep";
    }
    return "Selection Ready";
}

inline std::string build_world_workspace_viewport_focus_detail(const WorldWorkspaceState& state) {
    if (state.viewport_pan_active) {
        return "Pan canvas framing";
    }
    if (state.drag_active && world_workspace_state_starts_with(state, "Story Anchor:")) {
        return state.selection_summary;
    }
    if (state.drag_active && world_workspace_state_starts_with(state, "Region:")) {
        return state.selection_summary;
    }
    if (world_workspace_state_starts_with(state, "Story Anchor:")) {
        return state.selection_summary;
    }
    if (world_workspace_state_starts_with(state, "Region:")) {
        return state.selection_summary;
    }
    if (!state.hovered_story_anchor_id.empty()) {
        return "Story Anchor: " + state.hovered_story_anchor_id;
    }
    if (!state.hovered_region_id.empty()) {
        return "Region: " + state.hovered_region_id;
    }
    if (state.active_workspace_id == "anchor_workspace") {
        return "Browse triggers and activation zones";
    }
    return "Click region or anchor to inspect";
}

inline std::string build_world_workspace_viewport_gizmo_label(const WorldWorkspaceState& state) {
    const std::string gizmo_id = world_workspace_active_gizmo_id(state);
    if (state.viewport_pan_active) {
        return "Viewport Pan";
    }
    if (state.drag_active && gizmo_id == "anchor_radius") {
        return "Radius Handle Drag";
    }
    if (state.drag_active && gizmo_id == "region_resize") {
        return "Bounds Handle Drag";
    }
    if (state.drag_active && world_workspace_state_starts_with(state, "Story Anchor:")) {
        return "Radius Handle";
    }
    if (state.drag_active && world_workspace_state_starts_with(state, "Region:")) {
        return "Bounds Handle";
    }
    if (state.hovered_gizmo_id == "anchor_radius") {
        return "Radius Handle Hot";
    }
    if (state.hovered_gizmo_id == "region_resize") {
        return "Bounds Handle Hot";
    }
    if (world_workspace_state_starts_with(state, "Story Anchor:")) {
        return "Radius Handle";
    }
    if (world_workspace_state_starts_with(state, "Region:")) {
        return "Bounds Handle";
    }
    return "Navigation";
}

inline std::string build_world_workspace_viewport_gizmo_detail(const WorldWorkspaceState& state) {
    const std::string gizmo_id = world_workspace_active_gizmo_id(state);
    if (state.viewport_pan_active) {
        return "Canvas framing drag";
    }
    if (state.drag_active && gizmo_id == "anchor_radius") {
        return "Live radius adjustment";
    }
    if (state.drag_active && gizmo_id == "region_resize") {
        return "Live bounds adjustment";
    }
    if (state.drag_active && world_workspace_state_starts_with(state, "Story Anchor:")) {
        return "Live anchor adjustment";
    }
    if (state.drag_active && world_workspace_state_starts_with(state, "Region:")) {
        return "Live bounds adjustment";
    }
    if (state.hovered_gizmo_id == "anchor_radius") {
        return "Drag handle or [ ] radius";
    }
    if (state.hovered_gizmo_id == "region_resize") {
        return "Drag handle or Wheel resize";
    }
    if (world_workspace_state_starts_with(state, "Story Anchor:")) {
        return "Adjust activation radius";
    }
    if (world_workspace_state_starts_with(state, "Region:")) {
        return "Resize from bottom-right corner";
    }
    return "Select to reveal edit handles";
}

inline std::string build_world_workspace_viewport_ghost_label(const WorldWorkspaceState& state) {
    const std::string gizmo_id = world_workspace_active_gizmo_id(state);
    if (!state.drag_active) {
        return {};
    }
    if (gizmo_id == "anchor_radius" && state.drag_delta_x != 0.0F) {
        return "Radius Preview";
    }
    if (gizmo_id == "region_resize" && (state.drag_delta_x != 0.0F || state.drag_delta_y != 0.0F)) {
        return "Resize Preview";
    }
    if (world_workspace_state_starts_with(state, "Story Anchor:") && (state.drag_delta_x != 0.0F || state.drag_delta_y != 0.0F)) {
        return "Anchor Ghost";
    }
    if (world_workspace_state_starts_with(state, "Region:") && (state.drag_delta_x != 0.0F || state.drag_delta_y != 0.0F)) {
        return "Region Ghost";
    }
    return {};
}

inline std::string build_world_workspace_viewport_ghost_detail(const WorldWorkspaceState& state) {
    const std::string gizmo_id = world_workspace_active_gizmo_id(state);
    if (!state.drag_active || (state.drag_delta_x == 0.0F && state.drag_delta_y == 0.0F)) {
        return {};
    }
    if (gizmo_id == "anchor_radius") {
        return world_workspace_format_signed_scalar(state.drag_delta_x) + " radius";
    }
    if (gizmo_id == "region_resize") {
        return "delta " + world_workspace_format_scalar(state.drag_delta_x) + ", " + world_workspace_format_scalar(state.drag_delta_y);
    }
    return "offset " + world_workspace_format_scalar(state.drag_delta_x) + ", " + world_workspace_format_scalar(state.drag_delta_y);
}

inline std::array<std::string, 2> build_world_workspace_inspector_focus_badges(const WorldWorkspaceState& state) {
    std::string inspect_badge = "inspect:idle";
    if (world_workspace_state_starts_with(state, "Story Anchor:") || !state.hovered_story_anchor_id.empty()) {
        inspect_badge = "inspect:anchor";
    } else if (world_workspace_state_starts_with(state, "Region:") || !state.hovered_region_id.empty()) {
        inspect_badge = "inspect:region";
    }

    std::string state_badge = "state:track";
    if (world_workspace_state_starts_with(state, "Story Anchor:") || world_workspace_state_starts_with(state, "Region:")) {
        state_badge = "state:select";
    } else if (!state.hovered_story_anchor_id.empty() || !state.hovered_region_id.empty()) {
        state_badge = "state:hover";
    }
    return {inspect_badge, state_badge};
}

inline std::string build_world_workspace_inspector_focus_label(const WorldWorkspaceState& state) {
    if (world_workspace_state_starts_with(state, "Story Anchor:") || !state.hovered_story_anchor_id.empty()) {
        return "Anchor Inspector";
    }
    if (world_workspace_state_starts_with(state, "Region:") || !state.hovered_region_id.empty()) {
        return "Region Inspector";
    }
    return "Idle Inspector";
}

inline std::string build_world_workspace_inspector_focus_detail(const WorldWorkspaceState& state) {
    if (world_workspace_state_starts_with(state, "Story Anchor:")) {
        return "Selected: " + state.selection_summary.substr(std::string{"Story Anchor: "}.size());
    }
    if (world_workspace_state_starts_with(state, "Region:")) {
        return "Selected: " + state.selection_summary.substr(std::string{"Region: "}.size());
    }
    if (!state.hovered_story_anchor_id.empty()) {
        return "Hover: " + state.hovered_story_anchor_id;
    }
    if (!state.hovered_region_id.empty()) {
        return "Hover: " + state.hovered_region_id;
    }
    return "Selection follows hover or click";
}

inline int build_world_workspace_inspector_emphasis_index(const WorldWorkspaceState& state) {
    if (world_workspace_state_starts_with(state, "Story Anchor:") ||
        world_workspace_state_starts_with(state, "Region:") ||
        !state.hovered_story_anchor_id.empty() ||
        !state.hovered_region_id.empty()) {
        return 0;
    }
    return 2;
}

inline std::array<std::string, 2> build_world_workspace_viewport_hint_lines(const WorldWorkspaceState& state) {
    const std::string gizmo_id = world_workspace_active_gizmo_id(state);
    if (state.viewport_pan_active) {
        return {"Release mouse // pan viewport", "[F] reset // +/- zoom"};
    }
    if (state.drag_active && gizmo_id == "anchor_radius") {
        return {"Release mouse // commit radius", "[ ] radius // F5 save"};
    }
    if (state.drag_active && gizmo_id == "region_resize") {
        return {"Release mouse // commit bounds", "Wheel resize // F5 save"};
    }
    if (state.drag_active && world_workspace_state_starts_with(state, "Story Anchor:")) {
        return {"Release mouse // place anchor", "[ ] radius // F5 save"};
    }
    if (state.drag_active && world_workspace_state_starts_with(state, "Region:")) {
        return {"Release mouse // place region", "Wheel resize // F5 save"};
    }
    if (world_workspace_state_starts_with(state, "Story Anchor:")) {
        return {"Drag anchor // [ ] radius", "[Shift+F] frame // F5 save"};
    }
    if (world_workspace_state_starts_with(state, "Region:")) {
        return {"Drag region // Wheel resize", "[Shift+F] frame // F5 save"};
    }
    if (!state.hovered_story_anchor_id.empty()) {
        return {"Click anchor to inspect", "Drag after select // [ ] radius"};
    }
    if (!state.hovered_region_id.empty()) {
        return {"Click region to inspect", "Drag after select // Wheel resize"};
    }
    if (state.active_workspace_id == "anchor_workspace") {
        return {"Click anchor // Drag to place", "[ ] radius // F5 save"};
    }
    return {"Click select // Drag or WASD move", "Wheel / [ ] adjust // F5 save"};
}

inline WorldWorkspaceRenderModel build_world_workspace_render_model(
    const WorldWorkspaceState& state,
    const WorldWorkspaceViewModel& view_model
) {
    WorldWorkspaceRenderModel render_model{};
    render_model.header_subtitle = build_world_workspace_header_subtitle(state);
    render_model.workspace_tone = build_world_workspace_tone(state);
    render_model.rail_label = "// workspaces";
    render_model.page_title = build_world_workspace_page_title(state);
    render_model.page_subtitle = build_world_workspace_page_subtitle(state);
    render_model.page_context_badges = build_world_workspace_page_context_badges(state);
    render_model.shell_footer = build_world_workspace_shell_footer(state);
    render_model.header_tabs = build_world_workspace_header_tabs(state);
    render_model.header_status_badges = build_world_workspace_header_status_badges(view_model);
    render_model.rail_items = build_world_workspace_rail_items(state);
    render_model.action_badges = build_world_workspace_action_badges(state);
    render_model.viewport_heading = build_world_workspace_viewport_heading(state);
    render_model.viewport_badge = build_world_workspace_viewport_badge(state);
    render_model.viewport_frame_state = build_world_workspace_viewport_frame_state(state);
    render_model.viewport_frame_detail = build_world_workspace_viewport_frame_detail(state);
    render_model.viewport_meta_badges = build_world_workspace_viewport_meta_badges(state);
    render_model.viewport_focus_label = build_world_workspace_viewport_focus_label(state);
    render_model.viewport_focus_detail = build_world_workspace_viewport_focus_detail(state);
    render_model.viewport_hint_lines = build_world_workspace_viewport_hint_lines(state);
    render_model.viewport_gizmo_label = build_world_workspace_viewport_gizmo_label(state);
    render_model.viewport_gizmo_detail = build_world_workspace_viewport_gizmo_detail(state);
    render_model.viewport_ghost_label = build_world_workspace_viewport_ghost_label(state);
    render_model.viewport_ghost_detail = build_world_workspace_viewport_ghost_detail(state);
    render_model.inspector_focus_label = build_world_workspace_inspector_focus_label(state);
    render_model.inspector_focus_detail = build_world_workspace_inspector_focus_detail(state);
    render_model.inspector_focus_badges = build_world_workspace_inspector_focus_badges(state);
    render_model.inspector_emphasis_index = build_world_workspace_inspector_emphasis_index(state);
    render_model.toolbar_prefix = "selection //";
    render_model.toolbar_selection = state.selection_summary;
    render_model.toolbar_status = view_model.save_status_label;
    render_model.toolbar_context_badges = build_world_workspace_toolbar_context_badges(state);
    render_model.toolbar_tool_mode = build_world_workspace_tool_mode(state);
    render_model.toolbar_tool_cursor = build_world_workspace_tool_cursor(state);
    if (state.active_workspace_id == "anchor_workspace") {
        render_model.bottom_cards[0] = build_anchor_workspace_overview_card(state);
        render_model.bottom_cards[1] = build_anchor_workspace_audio_card(state);
        render_model.bottom_cards[2] = build_anchor_workspace_status_card(state, view_model);
        render_model.inspector_cards[0] = build_anchor_workspace_selection_card(state);
        render_model.inspector_cards[1] = build_anchor_workspace_runtime_card(state);
        render_model.inspector_cards[2] = build_world_workspace_session_render_card(view_model);
    } else {
        render_model.bottom_cards[0] = build_workspace_render_card(
            world_workspace_find_card(state, kWorldWorkspaceCardRegion),
            4U);
        render_model.bottom_cards[1] = build_workspace_render_card(
            world_workspace_find_card(state, kWorldWorkspaceCardAudio),
            4U);
        render_model.bottom_cards[2] = build_world_workspace_canvas_tools_card(state);
        render_model.inspector_cards[0] = build_world_workspace_inspector_render_card(state);
        render_model.inspector_cards[1] = build_workspace_render_card(
            world_workspace_find_card(state, kWorldWorkspaceCardTelemetry),
            4U);
        render_model.inspector_cards[2] = build_world_workspace_session_render_card(view_model);
    }
    render_model.properties_footer_badges = build_world_workspace_properties_footer_badges(state);
    render_model.properties_footer_lines = build_world_workspace_properties_footer_lines(state, view_model);
    render_model.inspector_cards[2].lines = {
        render_model.properties_footer_lines[0],
        render_model.properties_footer_lines[1],
        render_model.inspector_cards[2].lines[2],
        render_model.inspector_cards[2].lines[3],
    };
    return render_model;
}

}  // namespace resonance
