#include "engine/event/EventData.h"
#include "game/demo/DemoScene.h"
#include "game/demo/WorldWorkspaceLayout.h"
#include "game/demo/WorldWorkspaceRenderModel.h"
#include "game/demo/WorldWorkspaceViewModel.h"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <string>

namespace {

bool contains_rect(const resonance::WorkspaceRect& outer, const resonance::WorkspaceRect& inner) {
    return inner.x >= outer.x && inner.y >= outer.y &&
           (inner.x + inner.w) <= (outer.x + outer.w) &&
           (inner.y + inner.h) <= (outer.y + outer.h);
}

const resonance::RegionData* find_region(
    const std::vector<resonance::RegionData>& regions,
    const std::string& id
) {
    for (const auto& region : regions) {
        if (region.id == id) {
            return &region;
        }
    }
    return nullptr;
}

const resonance::StoryAnchorVisual* find_anchor_visual(
    const std::vector<resonance::StoryAnchorVisual>& anchors,
    const std::string& id
) {
    for (const auto& anchor : anchors) {
        if (anchor.id == id) {
            return &anchor;
        }
    }
    return nullptr;
}

std::string format_fixed(float value, int precision) {
    std::ostringstream stream;
    stream << std::fixed << std::setprecision(precision) << value;
    return stream.str();
}

std::string format_overlay_bounds(float x, float y, float width, float height) {
    return "Inspector Bounds: " + format_fixed(x, 1) + ", " + format_fixed(y, 1) + ", " +
        format_fixed(width, 1) + ", " + format_fixed(height, 1);
}

std::string format_overlay_position(float x, float y) {
    return "Selected Position: " + format_fixed(x, 1) + ", " + format_fixed(y, 1);
}

std::string format_overlay_radius(float value) {
    return "Selected Radius: " + format_fixed(value, 2);
}

std::string format_workspace_scalar(float value) {
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

std::string format_workspace_position(float x, float y) {
    return "Position: " + format_workspace_scalar(x) + ", " + format_workspace_scalar(y);
}

std::string format_metric_line(const std::string& label, float value) {
    return label + ": " + format_fixed(value, 2);
}

resonance::WorldPosition region_pick_point(const resonance::RegionData& region) {
    return resonance::WorldPosition{region.bounds.x + 1.0F, region.bounds.y + 1.0F};
}

resonance::WorldPosition region_handle_point(const resonance::RegionData& region) {
    return resonance::WorldPosition{region.bounds.x + region.bounds.width, region.bounds.y + region.bounds.height};
}

resonance::WorldPosition anchor_handle_point(const resonance::StoryAnchorVisual& anchor) {
    return resonance::WorldPosition{anchor.position.x + anchor.activation_radius, anchor.position.y};
}

}  // namespace

int main() {
    const auto workspace_layout = resonance::build_world_workspace_layout(
        resonance::kEditorWindowWidth,
        resonance::kEditorWindowHeight);
    const auto viewport_origin = resonance::to_world_workspace_position(
        static_cast<int>(workspace_layout.viewport_content.x),
        static_cast<int>(workspace_layout.viewport_content.y),
        true,
        resonance::kEditorWindowWidth,
        resonance::kEditorWindowHeight);
    const auto viewport_offset = resonance::to_world_workspace_position(
        static_cast<int>(workspace_layout.viewport_content.x + 42.0F),
        static_cast<int>(workspace_layout.viewport_content.y + 60.0F),
        true,
        resonance::kEditorWindowWidth,
        resonance::kEditorWindowHeight);
    const float viewport_center_x = workspace_layout.viewport_content.x + (workspace_layout.viewport_content.w * 0.5F);
    const float viewport_center_y = workspace_layout.viewport_content.y + (workspace_layout.viewport_content.h * 0.5F);

    const bool has_header_title = contains_rect(workspace_layout.header, workspace_layout.header_title);
    const bool has_active_world_tab = workspace_layout.header_tabs[0].active && workspace_layout.header_tabs[0].label == "World";
    const bool has_ordered_tabs = workspace_layout.header_tabs[0].rect.x < workspace_layout.header_tabs[1].rect.x &&
                                  workspace_layout.header_tabs[1].rect.x < workspace_layout.header_tabs[2].rect.x &&
                                  workspace_layout.header_tabs[2].rect.x < workspace_layout.header_tabs[3].rect.x;
    const bool has_tabs_in_header = std::all_of(workspace_layout.header_tabs.begin(), workspace_layout.header_tabs.end(), [&](const auto& tab) {
        return contains_rect(workspace_layout.header, tab.rect);
    });
    const bool has_header_tab_strip =
        contains_rect(workspace_layout.header, workspace_layout.header_tabs_strip) &&
        std::all_of(workspace_layout.header_tabs.begin(), workspace_layout.header_tabs.end(), [&](const auto& tab) {
            return contains_rect(workspace_layout.header_tabs_strip, tab.rect);
        });
    const bool has_prototype_header_tabs =
        workspace_layout.header_tabs_strip.x == 460.0F &&
        workspace_layout.header_tabs_strip.y == 12.0F &&
        workspace_layout.header_tabs_strip.w == 304.0F &&
        workspace_layout.header_tabs_strip.h == 36.0F &&
        workspace_layout.header_tabs[0].rect.x == 476.0F &&
        workspace_layout.header_tabs[0].rect.y == 16.0F &&
        workspace_layout.header_tabs[0].rect.w == 56.0F &&
        workspace_layout.header_tabs[0].rect.h == 24.0F &&
        workspace_layout.header_tabs[1].rect.x == 544.0F &&
        workspace_layout.header_tabs[2].rect.x == 612.0F &&
        workspace_layout.header_tabs[3].rect.x == 666.0F &&
        workspace_layout.header_tabs[1].rect.y == workspace_layout.header_tabs[0].rect.y &&
        workspace_layout.header_tabs[2].rect.y == workspace_layout.header_tabs[0].rect.y &&
        workspace_layout.header_tabs[3].rect.y == workspace_layout.header_tabs[0].rect.y;
    const bool has_prototype_shell_columns = workspace_layout.header.h == 72.0F &&
                                             workspace_layout.left_rail.x == 0.0F &&
                                             workspace_layout.left_rail.w == 220.0F &&
                                             workspace_layout.workspace_main.x == 220.0F &&
                                             workspace_layout.workspace_main.w == 900.0F &&
                                             workspace_layout.properties_panel.x == 1120.0F &&
                                             workspace_layout.properties_panel.w == 320.0F;
    const bool has_prototype_status_badges = workspace_layout.header_status[0].rect.w == 96.0F &&
                                             workspace_layout.header_status[1].rect.w == 80.0F &&
                                             workspace_layout.header_status[2].rect.w == 84.0F &&
                                             workspace_layout.header_status[0].rect.x < workspace_layout.header_status[1].rect.x &&
                                             workspace_layout.header_status[1].rect.x < workspace_layout.header_status[2].rect.x &&
                                             std::all_of(workspace_layout.header_status.begin(), workspace_layout.header_status.end(), [&](const auto& badge) {
                                                 return contains_rect(workspace_layout.header, badge.rect);
                                             });
    const bool has_header_status_strip =
        contains_rect(workspace_layout.header, workspace_layout.header_status_strip) &&
        std::all_of(workspace_layout.header_status.begin(), workspace_layout.header_status.end(), [&](const auto& badge) {
            return contains_rect(workspace_layout.header_status_strip, badge.rect);
        });
    const bool has_shell_separation =
        workspace_layout.left_rail.x + workspace_layout.left_rail.w <= workspace_layout.workspace_main.x &&
        workspace_layout.workspace_main.x + workspace_layout.workspace_main.w <= workspace_layout.properties_panel.x;
    const bool has_prototype_actions = contains_rect(workspace_layout.workspace_main, workspace_layout.page_title) &&
                                       contains_rect(workspace_layout.workspace_main, workspace_layout.page_subtitle) &&
                                       contains_rect(workspace_layout.workspace_main, workspace_layout.secondary_action) &&
                                       contains_rect(workspace_layout.workspace_main, workspace_layout.primary_action) &&
                                       workspace_layout.secondary_action.w == 96.0F &&
                                       workspace_layout.primary_action.w == 112.0F &&
                                       workspace_layout.secondary_action.x < workspace_layout.primary_action.x;
    const bool has_viewport_chrome = contains_rect(workspace_layout.viewport_frame, workspace_layout.viewport_toolbar) &&
                                     contains_rect(workspace_layout.viewport_frame, workspace_layout.viewport_content) &&
                                     workspace_layout.viewport_frame.w == 640.0F &&
                                     workspace_layout.viewport_frame.h == 412.0F &&
                                     workspace_layout.viewport_toolbar.y < workspace_layout.viewport_content.y;
    const bool has_viewport_scope_badge =
        contains_rect(workspace_layout.viewport_toolbar, workspace_layout.viewport_scope_badge) &&
        workspace_layout.viewport_scope_badge.x > workspace_layout.viewport_toolbar.x + (workspace_layout.viewport_toolbar.w * 0.55F);
    const bool has_viewport_focus_chip =
        contains_rect(workspace_layout.viewport_content, workspace_layout.viewport_focus_chip) &&
        workspace_layout.viewport_focus_chip.y < (workspace_layout.viewport_content.y + 56.0F);
    const bool has_viewport_hint_band =
        contains_rect(workspace_layout.viewport_content, workspace_layout.viewport_hint_band) &&
        workspace_layout.viewport_hint_band.y > (workspace_layout.viewport_content.y + workspace_layout.viewport_content.h - 64.0F);
    const bool has_viewport_gizmo_badge =
        contains_rect(workspace_layout.viewport_content, workspace_layout.viewport_gizmo_badge) &&
        workspace_layout.viewport_gizmo_badge.x > (workspace_layout.viewport_content.x + workspace_layout.viewport_content.w - 196.0F);
    const bool has_prototype_bottom_dock = contains_rect(workspace_layout.workspace_main, workspace_layout.bottom_dock) &&
                                           workspace_layout.bottom_dock.w == 640.0F &&
                                           workspace_layout.bottom_dock.h == 144.0F &&
                                           workspace_layout.bottom_dock.y > workspace_layout.viewport_frame.y + workspace_layout.viewport_frame.h;
    const bool has_bottom_cards_under_viewport = std::all_of(workspace_layout.bottom_cards.begin(), workspace_layout.bottom_cards.end(), [&](const auto& card) {
        return contains_rect(workspace_layout.bottom_dock, card) && card.w == 196.0F;
    });
    const bool has_inspector_stack = workspace_layout.inspector_cards[0].w == 296.0F &&
                                     workspace_layout.inspector_cards[1].w == 296.0F &&
                                     workspace_layout.inspector_cards[2].w == 296.0F &&
                                     workspace_layout.inspector_cards[0].y < workspace_layout.inspector_cards[1].y &&
                                     workspace_layout.inspector_cards[1].y < workspace_layout.inspector_cards[2].y;
    const bool has_properties_footer_band =
        contains_rect(workspace_layout.properties_panel, workspace_layout.properties_footer_band) &&
        workspace_layout.properties_footer_band.y > (workspace_layout.inspector_cards[2].y + workspace_layout.inspector_cards[2].h);
    const bool has_properties_focus_band =
        contains_rect(workspace_layout.properties_panel, workspace_layout.properties_focus_band) &&
        workspace_layout.properties_focus_band.y < workspace_layout.inspector_cards[0].y;
    const bool maps_viewport_origin = viewport_origin.x == 0.0F && viewport_origin.y == 0.0F;
    const bool maps_viewport_offset = viewport_offset.x == 42.0F && viewport_offset.y == 60.0F;
    const auto hit_world_tab = resonance::world_workspace_tab_id_at(
        static_cast<int>(workspace_layout.header_tabs[0].rect.x + 4.0F),
        static_cast<int>(workspace_layout.header_tabs[0].rect.y + 4.0F),
        resonance::kEditorWindowWidth,
        resonance::kEditorWindowHeight);
    const auto hit_anchor_tab = resonance::world_workspace_tab_id_at(
        static_cast<int>(workspace_layout.header_tabs[1].rect.x + 4.0F),
        static_cast<int>(workspace_layout.header_tabs[1].rect.y + 4.0F),
        resonance::kEditorWindowWidth,
        resonance::kEditorWindowHeight);
    const auto hit_empty_header = resonance::world_workspace_tab_id_at(32, 32, resonance::kEditorWindowWidth, resonance::kEditorWindowHeight);
    const bool maps_tab_hit_regions =
        hit_world_tab == "world_workspace" &&
        hit_anchor_tab == "anchor_workspace" &&
        hit_empty_header.empty();
    const bool maps_viewport_content_hit_regions =
        resonance::world_workspace_viewport_contains(
            static_cast<int>(workspace_layout.viewport_content.x + 24.0F),
            static_cast<int>(workspace_layout.viewport_content.y + 24.0F),
            resonance::kEditorWindowWidth,
            resonance::kEditorWindowHeight) &&
        !resonance::world_workspace_viewport_contains(
            static_cast<int>(workspace_layout.header_tabs[0].rect.x + 4.0F),
            static_cast<int>(workspace_layout.header_tabs[0].rect.y + 4.0F),
            resonance::kEditorWindowWidth,
            resonance::kEditorWindowHeight) &&
        !resonance::world_workspace_viewport_contains(
            static_cast<int>(workspace_layout.bottom_cards[0].x + 12.0F),
            static_cast<int>(workspace_layout.bottom_cards[0].y + 12.0F),
            resonance::kEditorWindowWidth,
            resonance::kEditorWindowHeight) &&
        !resonance::world_workspace_viewport_contains(
            static_cast<int>(workspace_layout.inspector_cards[0].x + 12.0F),
            static_cast<int>(workspace_layout.inspector_cards[0].y + 12.0F),
            resonance::kEditorWindowWidth,
            resonance::kEditorWindowHeight);

    const auto bundle = resonance::load_demo_content("assets/data");
    resonance::DemoScene scene(bundle);

    const auto* meadow_region_ptr = find_region(scene.regions(), "meadow");
    const auto meadow_anchor_visuals = scene.story_anchor_visuals();
    const auto* meadow_anchor_ptr = find_anchor_visual(meadow_anchor_visuals, "meadow-swing");
    if (meadow_region_ptr == nullptr || meadow_anchor_ptr == nullptr) {
        return 1;
    }

    const auto meadow_region = *meadow_region_ptr;
    const auto meadow_anchor = *meadow_anchor_ptr;

    const auto meadow_pick = region_pick_point(meadow_region);
    const auto meadow_handle_pick = region_handle_point(meadow_region);
    const auto meadow_anchor_pick = meadow_anchor.position;
    const auto meadow_anchor_handle_pick = anchor_handle_point(meadow_anchor);
    const auto meadow_bounds_line = format_overlay_bounds(
        meadow_region.bounds.x,
        meadow_region.bounds.y,
        meadow_region.bounds.width,
        meadow_region.bounds.height);
    const auto moved_region_bounds_line = format_overlay_bounds(
        meadow_region.bounds.x + 12.0F,
        meadow_region.bounds.y + 8.0F,
        meadow_region.bounds.width + 16.0F,
        meadow_region.bounds.height + 8.0F);
    const auto moved_region_commit_line = format_overlay_bounds(
        meadow_region.bounds.x + 12.0F,
        meadow_region.bounds.y + 8.0F,
        meadow_region.bounds.width,
        meadow_region.bounds.height);
    const auto resized_region_commit_line = format_overlay_bounds(
        meadow_region.bounds.x,
        meadow_region.bounds.y,
        meadow_region.bounds.width + 16.0F,
        meadow_region.bounds.height + 12.0F);
    const auto anchor_position_line = format_overlay_position(meadow_anchor.position.x, meadow_anchor.position.y);
    const auto anchor_radius_line = format_overlay_radius(meadow_anchor.activation_radius);
    const auto moved_anchor_position_line = format_overlay_position(meadow_anchor.position.x + 8.0F, meadow_anchor.position.y + 6.0F);
    const auto moved_anchor_radius_line = format_overlay_radius(meadow_anchor.activation_radius + 4.0F);
    const auto committed_anchor_radius_line = format_overlay_radius(meadow_anchor.activation_radius + 24.0F);
    const auto anchor_render_position_line = format_workspace_position(meadow_anchor.position.x, meadow_anchor.position.y);
    const auto manipulated_anchor_render_radius = "Activation Radius: " + format_workspace_scalar(meadow_anchor.activation_radius + 4.0F);

    scene.set_player_position(meadow_anchor_pick);
    scene.update();

    const auto lines = scene.overlay_lines();
    const bool has_mode_play_line = std::find(lines.begin(), lines.end(), std::string{"Mode: Play"}) != lines.end();
    const bool has_region_line = std::find(lines.begin(), lines.end(), std::string{"Region: "} + scene.current_region_id()) != lines.end();
    const bool has_music_line = std::find(lines.begin(), lines.end(), std::string{"Music: "} + scene.current_music_state()) != lines.end();
    const bool has_prompt_line = std::find(lines.begin(), lines.end(), std::string{"Action: "} + scene.current_interaction_prompt()) != lines.end();
    const bool has_nearby_anchor_line = std::find(lines.begin(), lines.end(), std::string{"Nearby Anchor: "} + meadow_anchor.id) != lines.end();
    const bool has_story_focus_line = std::find(lines.begin(), lines.end(), format_metric_line("Story Focus", scene.current_story_focus())) != lines.end();
    const bool has_event_emphasis_line = std::find(lines.begin(), lines.end(), format_metric_line("Event Emphasis", scene.current_event_emphasis())) != lines.end();
    const bool has_event_duck_line = std::find(lines.begin(), lines.end(), format_metric_line("Event Duck", scene.current_event_mix_profile().event_duck)) != lines.end();
    const bool has_ambient_boost_line = std::find(lines.begin(), lines.end(), format_metric_line("Ambient Boost", scene.current_event_mix_profile().ambient_boost)) != lines.end();
    const bool has_bgm_gain_line = std::find(lines.begin(), lines.end(), format_metric_line("Bgm Gain", scene.current_bgm_gain())) != lines.end();
    const bool has_ambient_gain_line = std::find(lines.begin(), lines.end(), format_metric_line("Ambient Gain", scene.current_ambient_gain_multiplier())) != lines.end();

    scene.toggle_editor_mode();
    scene.update();
    const auto edit_lines = scene.overlay_lines();
    const auto edit_state = scene.world_workspace_state();
    const auto edit_state_view_model = resonance::build_world_workspace_view_model(edit_state);
    const auto edit_render_model = resonance::build_world_workspace_render_model(edit_state, edit_state_view_model);
    const bool switched_to_anchor_workspace = scene.set_editor_workspace("anchor_workspace");
    scene.update();
    const auto anchor_workspace_lines = scene.overlay_lines();
    const auto anchor_workspace_state = scene.world_workspace_state();
    const auto anchor_workspace_view_model = resonance::build_world_workspace_view_model(anchor_workspace_state);
    const auto anchor_workspace_render_model = resonance::build_world_workspace_render_model(anchor_workspace_state, anchor_workspace_view_model);
    const bool has_anchor_workspace_state = anchor_workspace_state.active_workspace_id == "anchor_workspace";
    const bool has_anchor_workspace_header =
        anchor_workspace_render_model.header_tabs[0].label == "World" &&
        !anchor_workspace_render_model.header_tabs[0].active &&
        anchor_workspace_render_model.header_tabs[1].label == "Anchors" &&
        anchor_workspace_render_model.header_tabs[1].active;
    const bool has_anchor_workspace_copy =
        anchor_workspace_render_model.header_subtitle == "anchor_workspace // shell_focus" &&
        anchor_workspace_render_model.page_title == "ANCHOR WORKSPACE" &&
        anchor_workspace_render_model.page_subtitle == "shape story triggers, prompts, and activation zones";
    const bool has_anchor_workspace_context_badges =
        anchor_workspace_render_model.page_context_badges[0] == "workspace:anchor" &&
        anchor_workspace_render_model.page_context_badges[1] == "frame:view" &&
        anchor_workspace_render_model.page_context_badges[2] == "selection:none";
    const bool has_anchor_workspace_view_line = std::find(
        anchor_workspace_lines.begin(),
        anchor_workspace_lines.end(),
        std::string{"Workspace View: Anchor Canvas"}) != anchor_workspace_lines.end();
    const bool has_anchor_workspace_cards =
        anchor_workspace_render_model.bottom_cards[0].title == "anchor_overview" &&
        anchor_workspace_render_model.bottom_cards[1].title == "anchor_audio" &&
        anchor_workspace_render_model.bottom_cards[2].title == "anchor_status";
    const bool has_anchor_workspace_inspector_titles =
        anchor_workspace_render_model.inspector_cards[0].title == "anchor_selection" &&
        anchor_workspace_render_model.inspector_cards[1].title == "runtime_context" &&
        anchor_workspace_render_model.inspector_cards[2].title == "session_status";
    const bool has_anchor_workspace_chrome_tokens =
        anchor_workspace_render_model.workspace_tone == "anchor" &&
        anchor_workspace_render_model.viewport_badge == "Anchor Canvas";
    const bool has_anchor_workspace_viewport_feedback =
        anchor_workspace_render_model.viewport_focus_label == "Anchor Sweep" &&
        anchor_workspace_render_model.viewport_focus_detail == "Browse triggers and activation zones" &&
        anchor_workspace_render_model.viewport_hint_lines[0] == "Click anchor // Drag to place" &&
        anchor_workspace_render_model.viewport_hint_lines[1] == "[ ] radius // F5 save";
    const bool switched_back_to_world_workspace = scene.set_editor_workspace("world_workspace");
    scene.update();
    const bool has_mode_edit_line = std::find(edit_lines.begin(), edit_lines.end(), std::string{"Mode: Edit"}) != edit_lines.end();
    const bool has_edit_state_workspace = edit_state.active_workspace_id == "world_workspace";
    const bool has_edit_state_mode = edit_state.mode_label == "Edit";
    const bool has_edit_state_selection = edit_state.selection_summary == "No selection";
    const bool has_default_viewport_state =
        edit_state.viewport_origin_x == 0.0F &&
        edit_state.viewport_origin_y == 0.0F &&
        edit_state.viewport_zoom == 1.0F;
    const bool has_edit_state_view_model_dirty = edit_state_view_model.dirty_label == "dirty:false";
    const bool has_edit_state_view_model_mode = edit_state_view_model.mode_label == "mode:edit";
    const bool has_edit_state_view_model_workspace = edit_state_view_model.active_workspace_label == "active_workspace: world_workspace";
    const bool has_edit_state_view_model_session = edit_state_view_model.session_status_lines[0] == "document_mode: edit";
    const bool has_edit_state_view_model_selection = edit_state_view_model.selection_summary == "No selection";
    const bool has_edit_state_view_model_save = edit_state_view_model.save_status_label == "save:none";
    const bool has_default_viewport_overlay =
        std::find(edit_lines.begin(), edit_lines.end(), std::string{"Viewport Zoom: 1.00x"}) != edit_lines.end() &&
        std::find(edit_lines.begin(), edit_lines.end(), std::string{"Viewport Origin: 0.0, 0.0"}) != edit_lines.end() &&
        std::find(edit_lines.begin(), edit_lines.end(), std::string{"Viewport Pan: idle"}) != edit_lines.end();
    const bool has_world_workspace_line = std::find(edit_lines.begin(), edit_lines.end(), std::string{"Workspace: world_workspace"}) != edit_lines.end();
    const bool has_world_workspace_tabs_line = std::find(edit_lines.begin(), edit_lines.end(), std::string{"Workspace Tabs: World | Anchors | Events | Audio"}) != edit_lines.end();
    const bool has_world_workspace_view_line = std::find(edit_lines.begin(), edit_lines.end(), std::string{"Workspace View: World Canvas"}) != edit_lines.end();
    const bool has_world_workspace_cards_line = std::find(edit_lines.begin(), edit_lines.end(), std::string{"Workspace Cards: Region Summary | Audio Defaults | Canvas Tools"}) != edit_lines.end();
    const bool has_world_workspace_inspector_line = std::find(edit_lines.begin(), edit_lines.end(), std::string{"Workspace Inspector: region_identity | audio_defaults | runtime_telemetry"}) != edit_lines.end();
    const bool has_edit_render_bottom_titles =
        edit_render_model.bottom_cards[0].title == "region_summary" &&
        edit_render_model.bottom_cards[1].title == "audio_defaults" &&
        edit_render_model.bottom_cards[2].title == "canvas_tools";
    const bool has_edit_render_inspector_titles =
        edit_render_model.inspector_cards[0].title == "inspector" &&
        edit_render_model.inspector_cards[1].title == "runtime_telemetry" &&
        edit_render_model.inspector_cards[2].title == "session_status";
    const bool has_edit_render_session_line =
        std::find(edit_render_model.inspector_cards[2].lines.begin(), edit_render_model.inspector_cards[2].lines.end(), std::string{"No selection"}) != edit_render_model.inspector_cards[2].lines.end();
    const bool has_edit_render_rail =
        edit_render_model.rail_items[0].label == "world_region_editor" &&
        edit_render_model.rail_items[0].active &&
        edit_render_model.rail_items[1].label == "story_anchor_editor" &&
        !edit_render_model.rail_items[1].active;
    const bool has_edit_render_toolbar =
        edit_render_model.toolbar_prefix == "selection //" &&
        edit_render_model.toolbar_selection == "No selection" &&
        edit_render_model.toolbar_status == "save:none" &&
        edit_render_model.toolbar_context_badges[0] == "scope:view" &&
        edit_render_model.toolbar_context_badges[1] == "tool:select" &&
        edit_render_model.toolbar_context_badges[2] == "cursor:xhair";

    const bool has_edit_render_tooling =
        edit_render_model.toolbar_tool_mode == "tool:select" &&
        edit_render_model.toolbar_tool_cursor == "cursor:crosshair" &&
        edit_render_model.bottom_cards[2].lines[0] == "tool_select: active" &&
        edit_render_model.bottom_cards[2].lines[1] == "tool_move_bounds: enabled" &&
        edit_render_model.bottom_cards[2].lines[2] == "tool_toggle_anchors: enabled";
    const bool has_edit_render_actions =
        edit_render_model.action_badges[0] == "export_layout" &&
        edit_render_model.action_badges[1] == "playtest_route";
    const bool has_edit_render_shell_copy =
        edit_render_model.rail_label == "// workspaces" &&
        edit_render_model.page_title == "WORLD / REGION WORKSPACE" &&
        edit_render_model.page_subtitle == "edit bounds, ambience defaults, region tags, and playable context" &&
        edit_render_model.shell_footer == "world_workspace // runtime shell aligned to prototype";
    const bool has_edit_render_page_context_badges =
        edit_render_model.page_context_badges[0] == "workspace:world" &&
        edit_render_model.page_context_badges[1] == "frame:view" &&
        edit_render_model.page_context_badges[2] == "selection:none";
    const bool has_edit_render_header_tabs =
        edit_render_model.header_tabs[0].label == "World" &&
        edit_render_model.header_tabs[0].active &&
        edit_render_model.header_tabs[1].label == "Anchors" &&
        !edit_render_model.header_tabs[1].active;
    const bool has_edit_render_header_subtitle =
        edit_render_model.header_subtitle == "world_workspace // world_focus";
    const bool has_edit_render_header_status_badges =
        edit_render_model.header_status_badges[0] == "save:none" &&
        edit_render_model.header_status_badges[1] == "dirty:false" &&
        edit_render_model.header_status_badges[2] == "mode:edit";
    const bool has_edit_render_chrome_tokens =
        edit_render_model.workspace_tone == "world" &&
        edit_render_model.viewport_badge == "World Canvas";
    const bool has_edit_render_frame_cues =
        edit_render_model.viewport_frame_state == "frame:viewport_only" &&
        edit_render_model.viewport_frame_detail == "Edit inside canvas bounds";
    const bool has_edit_render_viewport_meta_badges =
        edit_render_model.viewport_meta_badges[0] == "zoom:1.00x" &&
        edit_render_model.viewport_meta_badges[1] == "origin:+0,+0" &&
        edit_render_model.viewport_meta_badges[2] == "pan:idle";
    const bool has_edit_render_viewport_feedback =
        edit_render_model.viewport_focus_label == "Selection Ready" &&
        edit_render_model.viewport_focus_detail == "Click region or anchor to inspect" &&
        edit_render_model.viewport_hint_lines[0] == "Click select // Drag or WASD move" &&
        edit_render_model.viewport_hint_lines[1] == "Wheel / [ ] adjust // F5 save";
    const bool has_edit_render_properties_footer =
        edit_render_model.properties_footer_lines[0] == "active_workspace: world_workspace" &&
        edit_render_model.properties_footer_lines[1] == "No selection" &&
        edit_render_model.properties_footer_badges[0] == "workspace:world" &&
        edit_render_model.properties_footer_badges[1] == "selection:none";
    const bool has_edit_render_inspector_band_badges =
        edit_render_model.inspector_focus_badges[0] == "inspect:idle" &&
        edit_render_model.inspector_focus_badges[1] == "state:track";
    const bool has_default_viewport_render =
        edit_render_model.bottom_cards[2].lines.size() >= 7U &&
        edit_render_model.bottom_cards[2].lines[3] == "view_zoom: 1.00x" &&
        edit_render_model.bottom_cards[2].lines[4] == "view_origin: +0, +0" &&
        edit_render_model.bottom_cards[2].lines[5] == "view_pan: enabled" &&
        edit_render_model.bottom_cards[2].lines[6] == "view_scope: viewport_only";
    const bool has_edit_render_inspector_focus =
        edit_render_model.inspector_focus_label == "Idle Inspector" &&
        edit_render_model.inspector_focus_detail == "Selection follows hover or click" &&
        edit_render_model.inspector_emphasis_index == 2 &&
        edit_render_model.viewport_gizmo_label == "Navigation" &&
        edit_render_model.viewport_gizmo_detail == "Select to reveal edit handles";
    const bool hovered_region = scene.set_editor_hover(meadow_pick);
    scene.update();
    const auto hovered_region_state = scene.world_workspace_state();
    const auto hovered_region_view_model = resonance::build_world_workspace_view_model(hovered_region_state);
    const auto hovered_region_render_model = resonance::build_world_workspace_render_model(hovered_region_state, hovered_region_view_model);
    const bool has_hovered_region_state =
        hovered_region &&
        scene.hovered_region_id() == "meadow" &&
        scene.hovered_story_anchor_id().empty() &&
        !scene.editor_drag_active();
    const bool has_hovered_region_render =
        hovered_region_render_model.viewport_focus_label == "Hover Region" &&
        hovered_region_render_model.viewport_focus_detail == "Region: meadow" &&
        hovered_region_render_model.viewport_hint_lines[0] == "Click region to inspect" &&
        hovered_region_render_model.viewport_hint_lines[1] == "Drag after select // Wheel resize" &&
        hovered_region_render_model.inspector_focus_label == "Region Inspector" &&
        hovered_region_render_model.inspector_focus_detail == "Hover: meadow" &&
        hovered_region_render_model.inspector_focus_badges[0] == "inspect:region" &&
        hovered_region_render_model.inspector_focus_badges[1] == "state:hover" &&
        hovered_region_render_model.inspector_emphasis_index == 0;
    const bool hovered_anchor = scene.set_editor_hover(meadow_anchor_pick);
    scene.update();
    const auto hovered_anchor_state = scene.world_workspace_state();
    const auto hovered_anchor_view_model = resonance::build_world_workspace_view_model(hovered_anchor_state);
    const auto hovered_anchor_render_model = resonance::build_world_workspace_render_model(hovered_anchor_state, hovered_anchor_view_model);
    const bool has_hovered_anchor_state =
        hovered_anchor &&
        scene.hovered_story_anchor_id() == "meadow-swing" &&
        scene.hovered_region_id().empty() &&
        !scene.editor_drag_active();
    const bool has_hovered_anchor_render =
        hovered_anchor_render_model.viewport_focus_label == "Hover Anchor" &&
        hovered_anchor_render_model.viewport_focus_detail == "Story Anchor: meadow-swing" &&
        hovered_anchor_render_model.viewport_hint_lines[0] == "Click anchor to inspect" &&
        hovered_anchor_render_model.viewport_hint_lines[1] == "Drag after select // [ ] radius" &&
        hovered_anchor_render_model.inspector_focus_label == "Anchor Inspector" &&
        hovered_anchor_render_model.inspector_focus_detail == "Hover: meadow-swing" &&
        hovered_anchor_render_model.inspector_emphasis_index == 0;
    const bool cleared_hover = scene.clear_editor_hover();
    scene.update();
    const auto cleared_hover_state = scene.world_workspace_state();
    const auto cleared_hover_view_model = resonance::build_world_workspace_view_model(cleared_hover_state);
    const auto cleared_hover_render_model = resonance::build_world_workspace_render_model(cleared_hover_state, cleared_hover_view_model);
    const bool has_cleared_hover_state =
        cleared_hover &&
        scene.hovered_region_id().empty() &&
        scene.hovered_story_anchor_id().empty() &&
        scene.hovered_gizmo_id().empty() &&
        !scene.editor_drag_active();
    const bool has_cleared_hover_render =
        cleared_hover_render_model.viewport_focus_label == "Selection Ready" &&
        cleared_hover_render_model.viewport_focus_detail == "Click region or anchor to inspect" &&
        cleared_hover_render_model.inspector_focus_label == "Idle Inspector" &&
        cleared_hover_render_model.inspector_focus_detail == "Selection follows hover or click";
    resonance::DemoScene navigated_scene(bundle);
    navigated_scene.toggle_editor_mode();
    const bool panned_viewport = navigated_scene.pan_editor_viewport({24.0F, -16.0F});
    const bool zoomed_viewport = navigated_scene.adjust_editor_viewport_zoom(0.25F);
    navigated_scene.update();
    const auto navigated_state = navigated_scene.world_workspace_state();
    const auto navigated_view_model = resonance::build_world_workspace_view_model(navigated_state);
    const auto navigated_render_model = resonance::build_world_workspace_render_model(navigated_state, navigated_view_model);
    const auto navigated_screen = resonance::to_world_workspace_screen_position(
        meadow_pick,
        navigated_scene.editor_viewport_origin(),
        navigated_scene.editor_viewport_zoom(),
        resonance::kEditorWindowWidth,
        resonance::kEditorWindowHeight);
    const auto remapped_world = resonance::to_world_workspace_position(
        static_cast<int>(std::lround(navigated_screen.x)),
        static_cast<int>(std::lround(navigated_screen.y)),
        navigated_scene.editor_viewport_origin(),
        navigated_scene.editor_viewport_zoom(),
        resonance::kEditorWindowWidth,
        resonance::kEditorWindowHeight);
    const bool has_navigated_viewport_state =
        panned_viewport &&
        zoomed_viewport &&
        navigated_state.viewport_origin_x == 24.0F &&
        navigated_state.viewport_origin_y == -16.0F &&
        navigated_state.viewport_zoom == 1.25F &&
        std::fabs(remapped_world.x - meadow_pick.x) < 1.0F &&
        std::fabs(remapped_world.y - meadow_pick.y) < 1.0F;
    const auto navigated_lines = navigated_scene.overlay_lines();
    const bool has_navigated_viewport_overlay =
        std::find(navigated_lines.begin(), navigated_lines.end(), std::string{"Viewport Zoom: 1.25x"}) != navigated_lines.end() &&
        std::find(navigated_lines.begin(), navigated_lines.end(), std::string{"Viewport Origin: 24.0, -16.0"}) != navigated_lines.end();
    const bool has_navigated_viewport_render =
        navigated_render_model.bottom_cards[2].lines.size() >= 5U &&
        navigated_render_model.bottom_cards[2].lines[3] == "view_zoom: 1.25x" &&
        navigated_render_model.bottom_cards[2].lines[4] == "view_origin: +24, -16";
    const bool has_navigated_viewport_meta_badges =
        navigated_render_model.viewport_meta_badges[0] == "zoom:1.25x" &&
        navigated_render_model.viewport_meta_badges[1] == "origin:+24,-16" &&
        navigated_render_model.viewport_meta_badges[2] == "pan:idle";

    resonance::DemoScene viewport_pan_scene(bundle);
    viewport_pan_scene.toggle_editor_mode();
    viewport_pan_scene.pan_editor_viewport({24.0F, -16.0F});
    viewport_pan_scene.adjust_editor_viewport_zoom(0.25F);
    viewport_pan_scene.set_editor_viewport_pan_active(true);
    viewport_pan_scene.update();
    const auto viewport_pan_lines = viewport_pan_scene.overlay_lines();
    const auto viewport_pan_state = viewport_pan_scene.world_workspace_state();
    const auto viewport_pan_view_model = resonance::build_world_workspace_view_model(viewport_pan_state);
    const auto viewport_pan_render_model = resonance::build_world_workspace_render_model(viewport_pan_state, viewport_pan_view_model);
    const bool has_viewport_pan_active_state =
        viewport_pan_scene.editor_viewport_pan_active() &&
        viewport_pan_state.viewport_pan_active &&
        std::find(viewport_pan_lines.begin(), viewport_pan_lines.end(), std::string{"Viewport Pan: active"}) != viewport_pan_lines.end();
    const bool has_viewport_pan_active_render =
        viewport_pan_render_model.toolbar_tool_mode == "tool:navigate_view" &&
        viewport_pan_render_model.toolbar_tool_cursor == "cursor:grabbing" &&
        viewport_pan_render_model.toolbar_context_badges[0] == "scope:nav" &&
        viewport_pan_render_model.toolbar_context_badges[1] == "tool:navigate" &&
        viewport_pan_render_model.toolbar_context_badges[2] == "cursor:grab" &&
        viewport_pan_render_model.viewport_focus_label == "Navigating View" &&
        viewport_pan_render_model.viewport_focus_detail == "Pan canvas framing" &&
        viewport_pan_render_model.viewport_frame_state == "frame:navigate_view" &&
        viewport_pan_render_model.viewport_frame_detail == "Viewport framing active" &&
        viewport_pan_render_model.viewport_hint_lines[0] == "Release mouse // pan viewport" &&
        viewport_pan_render_model.viewport_hint_lines[1] == "[F] reset // +/- zoom" &&
        viewport_pan_render_model.viewport_gizmo_label == "Viewport Pan" &&
        viewport_pan_render_model.viewport_gizmo_detail == "Canvas framing drag" &&
        viewport_pan_render_model.bottom_cards[2].lines.size() >= 7U &&
        viewport_pan_render_model.bottom_cards[2].lines[5] == "view_pan: active" &&
        viewport_pan_render_model.bottom_cards[2].lines[6] == "view_scope: navigate_view";
    const bool has_viewport_pan_meta_badges =
        viewport_pan_render_model.viewport_meta_badges[0] == "zoom:1.25x" &&
        viewport_pan_render_model.viewport_meta_badges[1] == "origin:+24,-16" &&
        viewport_pan_render_model.viewport_meta_badges[2] == "pan:active";
    viewport_pan_scene.set_editor_viewport_pan_active(false);
    const bool reset_viewport = viewport_pan_scene.reset_editor_viewport();
    viewport_pan_scene.update();
    const auto viewport_reset_lines = viewport_pan_scene.overlay_lines();
    const auto viewport_reset_state = viewport_pan_scene.world_workspace_state();
    const auto viewport_reset_view_model = resonance::build_world_workspace_view_model(viewport_reset_state);
    const auto viewport_reset_render_model = resonance::build_world_workspace_render_model(viewport_reset_state, viewport_reset_view_model);
    const bool has_viewport_reset_state =
        reset_viewport &&
        !viewport_pan_scene.editor_viewport_pan_active() &&
        viewport_reset_state.viewport_origin_x == 0.0F &&
        viewport_reset_state.viewport_origin_y == 0.0F &&
        viewport_reset_state.viewport_zoom == 1.0F &&
        !viewport_reset_state.viewport_pan_active &&
        std::find(viewport_reset_lines.begin(), viewport_reset_lines.end(), std::string{"Viewport Pan: idle"}) != viewport_reset_lines.end();
    const bool has_viewport_reset_render =
        viewport_reset_render_model.toolbar_tool_mode == "tool:select" &&
        viewport_reset_render_model.toolbar_tool_cursor == "cursor:crosshair" &&
        viewport_reset_render_model.bottom_cards[2].lines.size() >= 6U &&
        viewport_reset_render_model.bottom_cards[2].lines[5] == "view_pan: enabled";

    resonance::DemoScene focus_region_scene(bundle);
    focus_region_scene.toggle_editor_mode();
    focus_region_scene.pan_editor_viewport({180.0F, 96.0F});
    focus_region_scene.adjust_editor_viewport_zoom(-0.25F);
    const bool selected_region_for_focus = focus_region_scene.select_region_at(meadow_pick);
    const bool focused_region_viewport = focus_region_scene.focus_editor_viewport_on_selection();
    focus_region_scene.update();
    const auto focus_region_state = focus_region_scene.world_workspace_state();
    const resonance::WorldPosition region_center{
        meadow_region.bounds.x + (meadow_region.bounds.width * 0.5F),
        meadow_region.bounds.y + (meadow_region.bounds.height * 0.5F)};
    const auto focused_region_screen = resonance::to_world_workspace_screen_position(
        region_center,
        focus_region_scene.editor_viewport_origin(),
        focus_region_scene.editor_viewport_zoom(),
        resonance::kEditorWindowWidth,
        resonance::kEditorWindowHeight);
    const bool has_region_focus_viewport_control =
        selected_region_for_focus &&
        focused_region_viewport &&
        std::fabs(focused_region_screen.x - viewport_center_x) < 2.0F &&
        std::fabs(focused_region_screen.y - viewport_center_y) < 2.0F &&
        (std::fabs(focus_region_state.viewport_origin_x - 180.0F) > 0.1F ||
         std::fabs(focus_region_state.viewport_origin_y - 96.0F) > 0.1F ||
         std::fabs(focus_region_state.viewport_zoom - 0.75F) > 0.01F);

    resonance::DemoScene focus_anchor_scene(bundle);
    focus_anchor_scene.toggle_editor_mode();
    focus_anchor_scene.pan_editor_viewport({-48.0F, 120.0F});
    focus_anchor_scene.adjust_editor_viewport_zoom(-0.25F);
    const bool selected_anchor_for_focus = focus_anchor_scene.select_story_anchor_at(meadow_anchor_pick);
    const bool focused_anchor_viewport = focus_anchor_scene.focus_editor_viewport_on_selection();
    focus_anchor_scene.update();
    const auto focus_anchor_state = focus_anchor_scene.world_workspace_state();
    const auto focused_anchor_screen = resonance::to_world_workspace_screen_position(
        meadow_anchor_pick,
        focus_anchor_scene.editor_viewport_origin(),
        focus_anchor_scene.editor_viewport_zoom(),
        resonance::kEditorWindowWidth,
        resonance::kEditorWindowHeight);
    const bool has_anchor_focus_viewport_control =
        selected_anchor_for_focus &&
        focused_anchor_viewport &&
        std::fabs(focused_anchor_screen.x - viewport_center_x) < 2.0F &&
        std::fabs(focused_anchor_screen.y - viewport_center_y) < 2.0F &&
        (std::fabs(focus_anchor_state.viewport_origin_x + 48.0F) > 0.1F ||
         std::fabs(focus_anchor_state.viewport_origin_y - 120.0F) > 0.1F ||
         std::fabs(focus_anchor_state.viewport_zoom - 0.75F) > 0.01F);

    resonance::DemoScene zoom_reset_scene(bundle);
    zoom_reset_scene.toggle_editor_mode();
    zoom_reset_scene.pan_editor_viewport({88.0F, -36.0F});
    zoom_reset_scene.adjust_editor_viewport_zoom(0.25F);
    const bool reset_viewport_zoom_only = zoom_reset_scene.reset_editor_viewport_zoom();
    zoom_reset_scene.update();
    const auto zoom_reset_lines = zoom_reset_scene.overlay_lines();
    const auto zoom_reset_state = zoom_reset_scene.world_workspace_state();
    const bool has_zoom_reset_only_control =
        reset_viewport_zoom_only &&
        zoom_reset_state.viewport_origin_x == 88.0F &&
        zoom_reset_state.viewport_origin_y == -36.0F &&
        zoom_reset_state.viewport_zoom == 1.0F &&
        std::find(zoom_reset_lines.begin(), zoom_reset_lines.end(), std::string{"Viewport Zoom: 1.00x"}) != zoom_reset_lines.end() &&
        std::find(zoom_reset_lines.begin(), zoom_reset_lines.end(), std::string{"Viewport Origin: 88.0, -36.0"}) != zoom_reset_lines.end();

    const bool has_editor_controls_line = std::find(edit_lines.begin(), edit_lines.end(), std::string{"Editor Controls: Click select | Drag/WASD move | Wheel/[ ] size | Shift+F frame | 0 zoom | F5 save"}) != edit_lines.end();
    const bool has_editor_selection_none_line = std::find(edit_lines.begin(), edit_lines.end(), std::string{"Editor Selection: none"}) != edit_lines.end();

    const bool selected_region = scene.select_region_at(meadow_pick);
    scene.update();
    const auto region_edit_lines = scene.overlay_lines();
    const auto region_state = scene.world_workspace_state();
    const auto region_state_view_model = resonance::build_world_workspace_view_model(region_state);
    const auto region_render_model = resonance::build_world_workspace_render_model(region_state, region_state_view_model);
    const bool has_selected_region_line = std::find(region_edit_lines.begin(), region_edit_lines.end(), std::string{"Selected Region: meadow"}) != region_edit_lines.end();
    const bool has_region_state_selection = region_state.selection_summary == "Region: meadow";
    const bool has_region_state_view_model_selection = region_state_view_model.selection_summary == "Region: meadow";
    const bool has_region_state_view_model_save_clean = region_state_view_model.save_status_label == "save:none";
    const bool has_region_inspector_line = std::find(region_edit_lines.begin(), region_edit_lines.end(), std::string{"Inspector: Region"}) != region_edit_lines.end();
    const bool has_region_music_line = std::find(region_edit_lines.begin(), region_edit_lines.end(), std::string{"Inspector Music: explore"}) != region_edit_lines.end();
    const bool has_editor_dirty_clean_line = std::find(region_edit_lines.begin(), region_edit_lines.end(), std::string{"Editor Dirty: no"}) != region_edit_lines.end();
    const bool has_region_render_viewport_feedback =
        region_render_model.viewport_focus_label == "Region Focus" &&
        region_render_model.viewport_focus_detail == "Region: meadow" &&
        region_render_model.viewport_frame_state == "frame:selection_focus" &&
        region_render_model.viewport_frame_detail == "Handles active in canvas" &&
        region_render_model.viewport_hint_lines[0] == "Drag region // Wheel resize" &&
        region_render_model.viewport_hint_lines[1] == "[Shift+F] frame // F5 save" &&
        region_render_model.viewport_gizmo_label == "Bounds Handle" &&
        region_render_model.viewport_gizmo_detail == "Resize from bottom-right corner";
    const bool hovered_region_handle = scene.set_editor_hover(meadow_handle_pick);
    scene.update();
    const auto region_handle_state = scene.world_workspace_state();
    const auto region_handle_view_model = resonance::build_world_workspace_view_model(region_handle_state);
    const auto region_handle_render_model = resonance::build_world_workspace_render_model(region_handle_state, region_handle_view_model);
    const bool has_region_handle_hot_state =
        hovered_region_handle &&
        scene.hovered_gizmo_id() == "region_resize";
    const bool has_region_handle_hot_render =
        region_handle_render_model.viewport_gizmo_label == "Bounds Handle Hot" &&
        region_handle_render_model.viewport_gizmo_detail == "Drag handle or Wheel resize";

    const bool has_region_handle_tooling =
        region_handle_render_model.toolbar_tool_mode == "tool:resize_bounds" &&
        region_handle_render_model.toolbar_tool_cursor == "cursor:nwse-resize" &&
        region_handle_render_model.bottom_cards[2].lines[1] == "tool_move_bounds: hot";


    resonance::DemoScene region_move_preview_scene(bundle);
    region_move_preview_scene.toggle_editor_mode();
    region_move_preview_scene.update();
    const bool selected_region_for_move_preview = region_move_preview_scene.select_region_at(meadow_pick);
    region_move_preview_scene.set_editor_drag_active(true);
    const bool previewed_region_move = region_move_preview_scene.preview_editor_selection({12.0F, 8.0F});
    region_move_preview_scene.update();
    const auto region_move_preview_lines = region_move_preview_scene.overlay_lines();
    const auto region_move_preview_state = region_move_preview_scene.world_workspace_state();
    const auto region_move_preview_view_model = resonance::build_world_workspace_view_model(region_move_preview_state);
    const auto region_move_preview_render_model = resonance::build_world_workspace_render_model(region_move_preview_state, region_move_preview_view_model);
    const bool has_region_move_preview_behavior =
        selected_region_for_move_preview &&
        previewed_region_move &&
        region_move_preview_scene.editor_drag_active() &&
        std::find(region_move_preview_lines.begin(), region_move_preview_lines.end(), meadow_bounds_line) != region_move_preview_lines.end() &&
        region_move_preview_render_model.viewport_ghost_label == "Region Ghost" &&
        region_move_preview_render_model.viewport_ghost_detail == "offset 12, 8";
    const bool committed_region_move_preview = region_move_preview_scene.commit_editor_drag();
    region_move_preview_scene.update();
    const auto region_move_commit_lines = region_move_preview_scene.overlay_lines();
    const bool has_region_move_commit_behavior =
        committed_region_move_preview &&
        !region_move_preview_scene.editor_drag_active() &&
        std::find(region_move_commit_lines.begin(), region_move_commit_lines.end(), moved_region_commit_line) != region_move_commit_lines.end();

    resonance::DemoScene region_move_cancel_scene(bundle);
    region_move_cancel_scene.toggle_editor_mode();
    region_move_cancel_scene.update();
    const bool selected_region_for_move_cancel = region_move_cancel_scene.select_region_at(meadow_pick);
    region_move_cancel_scene.set_editor_drag_active(true);
    const bool previewed_region_move_cancel = region_move_cancel_scene.preview_editor_selection({20.0F, 10.0F});
    const bool canceled_region_move_preview = region_move_cancel_scene.cancel_editor_drag();
    region_move_cancel_scene.update();
    const auto region_move_cancel_lines = region_move_cancel_scene.overlay_lines();
    const bool has_region_move_cancel_behavior =
        selected_region_for_move_cancel &&
        previewed_region_move_cancel &&
        canceled_region_move_preview &&
        !region_move_cancel_scene.editor_drag_active() &&
        std::find(region_move_cancel_lines.begin(), region_move_cancel_lines.end(), meadow_bounds_line) != region_move_cancel_lines.end();

    resonance::DemoScene region_handle_drag_scene(bundle);
    region_handle_drag_scene.toggle_editor_mode();
    region_handle_drag_scene.update();
    const bool selected_region_for_handle_drag = region_handle_drag_scene.select_region_at(meadow_pick);
    const bool hovered_region_handle_for_handle_drag = region_handle_drag_scene.set_editor_hover(meadow_handle_pick);
    region_handle_drag_scene.set_editor_drag_active(true);
    region_handle_drag_scene.set_editor_hover({meadow_handle_pick.x + 16.0F, meadow_handle_pick.y + 12.0F});
    const bool previewed_region_by_handle_drag = region_handle_drag_scene.preview_editor_selection({16.0F, 12.0F});
    region_handle_drag_scene.update();
    const auto region_handle_drag_lines = region_handle_drag_scene.overlay_lines();
    const auto region_handle_drag_state = region_handle_drag_scene.world_workspace_state();
    const auto region_handle_drag_view_model = resonance::build_world_workspace_view_model(region_handle_drag_state);
    const auto region_handle_drag_render_model = resonance::build_world_workspace_render_model(region_handle_drag_state, region_handle_drag_view_model);
    const bool has_region_handle_drag_behavior =
        selected_region_for_handle_drag &&
        hovered_region_handle_for_handle_drag &&
        region_handle_drag_scene.editor_drag_active() &&
        previewed_region_by_handle_drag &&
        std::find(region_handle_drag_lines.begin(), region_handle_drag_lines.end(), meadow_bounds_line) != region_handle_drag_lines.end();
    const bool has_region_handle_drag_render =
        region_handle_drag_render_model.viewport_focus_label == "Resizing Region" &&
        region_handle_drag_render_model.viewport_focus_detail == "Region: meadow" &&
        region_handle_drag_render_model.viewport_hint_lines[0] == "Release mouse // commit bounds" &&
        region_handle_drag_render_model.viewport_hint_lines[1] == "Wheel resize // F5 save" &&
        region_handle_drag_render_model.viewport_gizmo_label == "Bounds Handle Drag" &&
        region_handle_drag_render_model.viewport_gizmo_detail == "Live bounds adjustment" &&
        region_handle_drag_render_model.viewport_ghost_label == "Resize Preview" &&
        region_handle_drag_render_model.viewport_ghost_detail == "delta 16, 12";

    const bool has_region_handle_drag_tooling =
        region_handle_drag_render_model.toolbar_tool_mode == "tool:resize_bounds" &&
        region_handle_drag_render_model.toolbar_tool_cursor == "cursor:nwse-resize" &&
        region_handle_drag_render_model.bottom_cards[2].lines[1] == "tool_move_bounds: active";

    const bool committed_region_handle_drag = region_handle_drag_scene.commit_editor_drag();
    region_handle_drag_scene.update();
    const auto region_handle_drag_commit_lines = region_handle_drag_scene.overlay_lines();
    const bool has_region_handle_drag_commit_behavior =
        committed_region_handle_drag &&
        !region_handle_drag_scene.editor_drag_active() &&
        std::find(region_handle_drag_commit_lines.begin(), region_handle_drag_commit_lines.end(), resized_region_commit_line) != region_handle_drag_commit_lines.end();

    const bool moved_region = scene.nudge_editor_selection({12.0F, 8.0F});
    const bool resized_region = scene.adjust_editor_selection_primary(2.0F);
    scene.update();
    const auto moved_region_lines = scene.overlay_lines();
    const auto moved_region_state = scene.world_workspace_state();
    const auto moved_region_state_view_model = resonance::build_world_workspace_view_model(moved_region_state);
    const auto moved_region_render_model = resonance::build_world_workspace_render_model(moved_region_state, moved_region_state_view_model);
    const bool has_region_bounds_line = std::find(moved_region_lines.begin(), moved_region_lines.end(), moved_region_bounds_line) != moved_region_lines.end();
    const bool has_editor_dirty_yes_line = std::find(moved_region_lines.begin(), moved_region_lines.end(), std::string{"Editor Dirty: yes"}) != moved_region_lines.end();
    const bool has_moved_region_state_view_model_dirty = moved_region_state_view_model.dirty_label == "dirty:true";
    const bool has_moved_region_state_view_model_save = moved_region_state_view_model.save_status_label == "save:modified";
    const bool has_moved_region_header_status_badges =
        moved_region_render_model.header_status_badges[0] == "save:modified" &&
        moved_region_render_model.header_status_badges[1] == "dirty:true" &&
        moved_region_render_model.header_status_badges[2] == "mode:edit";

    const bool selected_anchor = scene.select_story_anchor_at(meadow_anchor_pick);
    scene.update();
    const auto selected_edit_lines = scene.overlay_lines();
    const auto anchor_state = scene.world_workspace_state();
    const auto anchor_state_view_model = resonance::build_world_workspace_view_model(anchor_state);
    const auto anchor_render_model = resonance::build_world_workspace_render_model(anchor_state, anchor_state_view_model);
    const bool has_selected_anchor_line = std::find(selected_edit_lines.begin(), selected_edit_lines.end(), std::string{"Selected Anchor: meadow-swing"}) != selected_edit_lines.end();
    const bool has_anchor_state_selection = anchor_state.selection_summary == "Story Anchor: meadow-swing";
    const bool has_anchor_state_view_model_selection = anchor_state_view_model.selection_summary == "Story Anchor: meadow-swing";
    const bool has_anchor_state_view_model_save = anchor_state_view_model.save_status_label == "save:modified";
    const bool has_anchor_inspector_line = std::find(selected_edit_lines.begin(), selected_edit_lines.end(), std::string{"Inspector: Story Anchor"}) != selected_edit_lines.end();
    const bool has_anchor_region_line = std::find(selected_edit_lines.begin(), selected_edit_lines.end(), std::string{"Inspector Region: meadow"}) != selected_edit_lines.end();
    const bool has_anchor_render_title = anchor_render_model.inspector_cards[0].title == "inspector";
    const bool has_anchor_render_position = std::find(
        anchor_render_model.inspector_cards[0].lines.begin(),
        anchor_render_model.inspector_cards[0].lines.end(),
        anchor_render_position_line) != anchor_render_model.inspector_cards[0].lines.end();
    const bool has_anchor_render_toolbar =
        anchor_render_model.toolbar_selection == "Story Anchor: meadow-swing" &&
        anchor_render_model.toolbar_status == "save:modified" &&
        anchor_render_model.toolbar_context_badges[0] == "scope:select" &&
        anchor_render_model.toolbar_context_badges[1] == "tool:move" &&
        anchor_render_model.toolbar_context_badges[2] == "cursor:pointer";
    const bool has_anchor_render_action = anchor_render_model.action_badges[1] == "focus_anchor";
    const bool has_anchor_render_page_heading =
        anchor_render_model.page_title == "STORY ANCHOR // meadow-swing" &&
        anchor_render_model.page_subtitle == "anchor focus // meadow-swing in meadow";
    const bool has_anchor_render_page_context_badges =
        anchor_render_model.page_context_badges[0] == "workspace:world" &&
        anchor_render_model.page_context_badges[1] == "frame:select" &&
        anchor_render_model.page_context_badges[2] == "selection:anchor";
    const bool has_anchor_render_header_tabs =
        anchor_render_model.header_tabs[0].label == "World" &&
        anchor_render_model.header_tabs[0].active &&
        anchor_render_model.header_tabs[1].label == "Anchors" &&
        !anchor_render_model.header_tabs[1].active;
    const bool has_anchor_render_header_subtitle =
        anchor_render_model.header_subtitle == "world_workspace // world_focus";
    const bool has_anchor_render_properties_footer =
        anchor_render_model.properties_footer_lines[0] == "active_workspace: world_workspace" &&
        anchor_render_model.properties_footer_lines[1] == "save:modified" &&
        anchor_render_model.properties_footer_badges[0] == "workspace:world" &&
        anchor_render_model.properties_footer_badges[1] == "selection:anchor";
    const bool has_anchor_render_inspector_band_badges =
        anchor_render_model.inspector_focus_badges[0] == "inspect:anchor" &&
        anchor_render_model.inspector_focus_badges[1] == "state:select";
    const bool has_anchor_render_viewport_feedback =
        anchor_render_model.viewport_focus_label == "Anchor Focus" &&
        anchor_render_model.viewport_focus_detail == "Story Anchor: meadow-swing" &&
        anchor_render_model.viewport_frame_state == "frame:selection_focus" &&
        anchor_render_model.viewport_frame_detail == "Handles active in canvas" &&
        anchor_render_model.viewport_hint_lines[0] == "Drag anchor // [ ] radius" &&
        anchor_render_model.viewport_hint_lines[1] == "[Shift+F] frame // F5 save" &&
        anchor_render_model.viewport_gizmo_label == "Radius Handle" &&
        anchor_render_model.viewport_gizmo_detail == "Adjust activation radius";
    const bool hovered_anchor_handle = scene.set_editor_hover(meadow_anchor_handle_pick);
    scene.update();
    const auto anchor_handle_state = scene.world_workspace_state();
    const auto anchor_handle_view_model = resonance::build_world_workspace_view_model(anchor_handle_state);
    const auto anchor_handle_render_model = resonance::build_world_workspace_render_model(anchor_handle_state, anchor_handle_view_model);
    const bool has_anchor_handle_hot_state =
        hovered_anchor_handle &&
        scene.hovered_gizmo_id() == "anchor_radius";
    const bool has_anchor_handle_hot_render =
        anchor_handle_render_model.viewport_gizmo_label == "Radius Handle Hot" &&
        anchor_handle_render_model.viewport_gizmo_detail == "Drag handle or [ ] radius";

    const bool has_anchor_handle_tooling =
        anchor_handle_render_model.toolbar_tool_mode == "tool:adjust_radius" &&
        anchor_handle_render_model.toolbar_tool_cursor == "cursor:ew-resize";
    scene.set_editor_drag_active(true);
    scene.set_editor_drag_delta({8.0F, 6.0F});
    scene.update();
    const auto dragging_anchor_state_view_model = resonance::build_world_workspace_view_model(scene.world_workspace_state());
    const auto dragging_anchor_render_model = resonance::build_world_workspace_render_model(scene.world_workspace_state(), dragging_anchor_state_view_model);
    const bool has_dragging_anchor_state = scene.editor_drag_active() &&
                                           scene.editor_drag_delta().x == 8.0F &&
                                           scene.editor_drag_delta().y == 6.0F;
    const bool has_dragging_anchor_render =
        dragging_anchor_render_model.viewport_focus_label == "Adjusting Radius" &&
        dragging_anchor_render_model.viewport_focus_detail == "Story Anchor: meadow-swing" &&
        dragging_anchor_render_model.viewport_hint_lines[0] == "Release mouse // commit radius" &&
        dragging_anchor_render_model.viewport_hint_lines[1] == "[ ] radius // F5 save" &&
        dragging_anchor_render_model.inspector_focus_label == "Anchor Inspector" &&
        dragging_anchor_render_model.inspector_focus_detail == "Selected: meadow-swing" &&
        dragging_anchor_render_model.viewport_gizmo_label == "Radius Handle Drag" &&
        dragging_anchor_render_model.viewport_gizmo_detail == "Live radius adjustment" &&
        dragging_anchor_render_model.viewport_ghost_label == "Radius Preview" &&
        dragging_anchor_render_model.viewport_ghost_detail == "+8 radius";

    const bool has_dragging_anchor_tooling =
        dragging_anchor_render_model.toolbar_tool_mode == "tool:adjust_radius" &&
        dragging_anchor_render_model.toolbar_tool_cursor == "cursor:ew-resize";
    scene.set_editor_drag_active(false);
    scene.set_editor_drag_delta({0.0F, 0.0F});


    resonance::DemoScene anchor_handle_drag_scene(bundle);
    anchor_handle_drag_scene.toggle_editor_mode();
    anchor_handle_drag_scene.update();
    const bool selected_anchor_for_handle_drag = anchor_handle_drag_scene.select_story_anchor_at(meadow_anchor_pick);
    const bool hovered_anchor_handle_for_handle_drag = anchor_handle_drag_scene.set_editor_hover(meadow_anchor_handle_pick);
    anchor_handle_drag_scene.set_editor_drag_active(true);
    anchor_handle_drag_scene.set_editor_hover({meadow_anchor_handle_pick.x + 24.0F, meadow_anchor_handle_pick.y});
    const bool previewed_anchor_by_handle_drag = anchor_handle_drag_scene.preview_editor_selection({24.0F, 0.0F});
    anchor_handle_drag_scene.update();
    const auto anchor_handle_drag_lines = anchor_handle_drag_scene.overlay_lines();
    const auto anchor_handle_drag_state = anchor_handle_drag_scene.world_workspace_state();
    const auto anchor_handle_drag_view_model = resonance::build_world_workspace_view_model(anchor_handle_drag_state);
    const auto anchor_handle_drag_render_model = resonance::build_world_workspace_render_model(anchor_handle_drag_state, anchor_handle_drag_view_model);
    const bool has_anchor_handle_drag_behavior =
        selected_anchor_for_handle_drag &&
        hovered_anchor_handle_for_handle_drag &&
        anchor_handle_drag_scene.editor_drag_active() &&
        previewed_anchor_by_handle_drag &&
        std::find(anchor_handle_drag_lines.begin(), anchor_handle_drag_lines.end(), anchor_position_line) != anchor_handle_drag_lines.end() &&
        std::find(anchor_handle_drag_lines.begin(), anchor_handle_drag_lines.end(), anchor_radius_line) != anchor_handle_drag_lines.end();
    const bool has_anchor_handle_drag_persisted_render =
        anchor_handle_drag_render_model.viewport_focus_label == "Adjusting Radius" &&
        anchor_handle_drag_render_model.viewport_focus_detail == "Story Anchor: meadow-swing" &&
        anchor_handle_drag_render_model.viewport_hint_lines[0] == "Release mouse // commit radius" &&
        anchor_handle_drag_render_model.viewport_hint_lines[1] == "[ ] radius // F5 save" &&
        anchor_handle_drag_render_model.viewport_gizmo_label == "Radius Handle Drag" &&
        anchor_handle_drag_render_model.viewport_gizmo_detail == "Live radius adjustment" &&
        anchor_handle_drag_render_model.viewport_ghost_label == "Radius Preview" &&
        anchor_handle_drag_render_model.viewport_ghost_detail == "+24 radius";

    const bool committed_anchor_handle_drag = anchor_handle_drag_scene.commit_editor_drag();
    anchor_handle_drag_scene.update();
    const auto anchor_handle_drag_commit_lines = anchor_handle_drag_scene.overlay_lines();
    const bool has_anchor_handle_drag_commit_behavior =
        committed_anchor_handle_drag &&
        !anchor_handle_drag_scene.editor_drag_active() &&
        std::find(anchor_handle_drag_commit_lines.begin(), anchor_handle_drag_commit_lines.end(), committed_anchor_radius_line) != anchor_handle_drag_commit_lines.end();

    const bool moved_anchor = scene.nudge_editor_selection({8.0F, 6.0F});
    const bool resized_anchor = scene.adjust_editor_selection_primary(2.0F);
    scene.update();
    const auto manipulated_edit_lines = scene.overlay_lines();
    const auto manipulated_anchor_state_view_model = resonance::build_world_workspace_view_model(scene.world_workspace_state());
    const auto manipulated_anchor_render_model = resonance::build_world_workspace_render_model(scene.world_workspace_state(), manipulated_anchor_state_view_model);
    const bool has_selected_position_line = std::find(manipulated_edit_lines.begin(), manipulated_edit_lines.end(), moved_anchor_position_line) != manipulated_edit_lines.end();
    const bool has_selected_radius_line = std::find(manipulated_edit_lines.begin(), manipulated_edit_lines.end(), moved_anchor_radius_line) != manipulated_edit_lines.end();
    const bool has_manipulated_anchor_state_view_model_dirty = manipulated_anchor_state_view_model.dirty_label == "dirty:true";
    const bool has_manipulated_anchor_state_view_model_save = manipulated_anchor_state_view_model.save_status_label == "save:modified";
    const bool has_manipulated_anchor_render_radius = std::find(
        manipulated_anchor_render_model.inspector_cards[0].lines.begin(),
        manipulated_anchor_render_model.inspector_cards[0].lines.end(),
        manipulated_anchor_render_radius) != manipulated_anchor_render_model.inspector_cards[0].lines.end();

    const bool cleared_selection = scene.clear_editor_selection();
    scene.update();
    const auto cleared_selection_lines = scene.overlay_lines();
    const auto cleared_state_view_model = resonance::build_world_workspace_view_model(scene.world_workspace_state());
    const bool has_cleared_selection_line = std::find(cleared_selection_lines.begin(), cleared_selection_lines.end(), std::string{"Editor Selection: none"}) != cleared_selection_lines.end();
    const bool cleared_selected_region_line = std::find_if(cleared_selection_lines.begin(), cleared_selection_lines.end(), [](const std::string& line) {
        return line.find("Selected Region:") == 0;
    }) == cleared_selection_lines.end();
    const bool cleared_selected_anchor_line = std::find_if(cleared_selection_lines.begin(), cleared_selection_lines.end(), [](const std::string& line) {
        return line.find("Selected Anchor:") == 0;
    }) == cleared_selection_lines.end();
    const bool has_cleared_state_view_model_selection = cleared_state_view_model.selection_summary == "No selection";
    const bool has_cleared_state_view_model_save = cleared_state_view_model.save_status_label == "save:modified";

    scene.toggle_editor_mode();
    scene.update();
    scene.interact();
    scene.update();

    const auto story_lines = scene.overlay_lines();
    const bool has_story_line = std::any_of(story_lines.begin(), story_lines.end(), [](const std::string& line) {
        return line.find("Story: ") == 0;
    });
    const bool has_active_anchor_line = std::find(story_lines.begin(), story_lines.end(), std::string{"Active Anchor: "} + meadow_anchor.id) != story_lines.end();

    scene.set_player_position({999.0F, 999.0F});
    scene.update();

    return (has_header_title &&
            has_active_world_tab &&
            has_ordered_tabs &&
            has_tabs_in_header &&
            has_header_tab_strip &&
            has_prototype_header_tabs &&
            has_prototype_shell_columns &&
            has_prototype_status_badges &&
            has_header_status_strip &&
            has_shell_separation &&
            has_viewport_chrome &&
            has_viewport_scope_badge &&
            has_viewport_focus_chip &&
            has_viewport_hint_band &&
            has_viewport_gizmo_badge &&
            has_prototype_actions &&
            has_bottom_cards_under_viewport &&
            has_prototype_bottom_dock &&
            has_inspector_stack &&
            has_properties_footer_band &&
            has_properties_focus_band &&
            maps_viewport_origin &&
            maps_viewport_offset &&
            maps_tab_hit_regions &&
            scene.current_region_id() == "" &&
            scene.current_music_state() == "" &&
            scene.current_event_id() == "" &&
            scene.current_interaction_prompt() == "" &&
            has_mode_play_line &&
            has_region_line &&
            has_music_line &&
            has_prompt_line &&
            has_nearby_anchor_line &&
            has_story_focus_line &&
            has_event_emphasis_line &&
            has_event_duck_line &&
            has_ambient_boost_line &&
            has_bgm_gain_line &&
            has_ambient_gain_line &&
            has_mode_edit_line &&
            has_edit_state_workspace &&
            switched_to_anchor_workspace &&
            has_anchor_workspace_state &&
            has_anchor_workspace_header &&
            has_anchor_workspace_copy &&
            has_anchor_workspace_context_badges &&
            has_anchor_workspace_view_line &&
            has_anchor_workspace_cards &&
            has_anchor_workspace_inspector_titles &&
            has_anchor_workspace_chrome_tokens &&
            has_anchor_workspace_viewport_feedback &&
            switched_back_to_world_workspace &&
            has_edit_state_mode &&
            has_edit_state_selection &&
            has_default_viewport_state &&
            has_edit_state_view_model_dirty &&
            has_edit_state_view_model_mode &&
            has_edit_state_view_model_workspace &&
            has_edit_state_view_model_session &&
            has_edit_state_view_model_selection &&
            has_edit_state_view_model_save &&
            has_default_viewport_overlay &&
            has_world_workspace_line &&
            has_world_workspace_tabs_line &&
            has_world_workspace_view_line &&
            has_world_workspace_cards_line &&
            has_world_workspace_inspector_line &&
            has_edit_render_bottom_titles &&
            has_edit_render_inspector_titles &&
            has_edit_render_session_line &&
            has_edit_render_rail &&
            has_edit_render_toolbar &&
            has_edit_render_tooling &&
            has_edit_render_actions &&
            has_edit_render_shell_copy &&
            has_edit_render_page_context_badges &&
            has_edit_render_header_tabs &&
            has_edit_render_header_subtitle &&
            has_edit_render_header_status_badges &&
            has_edit_render_chrome_tokens &&
            has_edit_render_frame_cues &&
            has_edit_render_viewport_meta_badges &&
            has_edit_render_viewport_feedback &&
            has_edit_render_inspector_focus &&
            has_edit_render_inspector_band_badges &&
            has_edit_render_properties_footer &&
            has_default_viewport_render &&
            has_navigated_viewport_state &&
            has_navigated_viewport_overlay &&
            has_navigated_viewport_render &&
            has_navigated_viewport_meta_badges &&
            has_viewport_pan_active_state &&
            has_viewport_pan_active_render &&
            has_viewport_pan_meta_badges &&
            has_viewport_reset_state &&
            has_viewport_reset_render &&
            has_region_focus_viewport_control &&
            has_anchor_focus_viewport_control &&
            has_zoom_reset_only_control &&
            maps_viewport_content_hit_regions &&
            has_hovered_region_state &&
            has_hovered_region_render &&
            has_hovered_anchor_state &&
            has_hovered_anchor_render &&
            has_cleared_hover_state &&
            has_cleared_hover_render &&
            has_editor_controls_line &&
            has_editor_selection_none_line &&
            selected_region &&
            has_selected_region_line &&
            has_region_state_selection &&
            has_region_state_view_model_selection &&
            has_region_state_view_model_save_clean &&
            has_region_inspector_line &&
            has_region_music_line &&
            has_editor_dirty_clean_line &&
            has_region_render_viewport_feedback &&
            has_region_handle_hot_state &&
            has_region_handle_hot_render &&
            has_region_handle_tooling &&
            has_region_move_preview_behavior &&
            has_region_move_commit_behavior &&
            has_region_move_cancel_behavior &&
            has_region_handle_drag_behavior &&
            has_region_handle_drag_render &&
            has_region_handle_drag_tooling &&
            has_region_handle_drag_commit_behavior &&
            moved_region &&
            resized_region &&
            has_region_bounds_line &&
            has_editor_dirty_yes_line &&
            has_moved_region_state_view_model_dirty &&
            has_moved_region_state_view_model_save &&
            has_moved_region_header_status_badges &&
            selected_anchor &&
            has_selected_anchor_line &&
            has_anchor_state_selection &&
            has_anchor_state_view_model_selection &&
            has_anchor_state_view_model_save &&
            has_anchor_inspector_line &&
            has_anchor_region_line &&
            has_anchor_render_title &&
            has_anchor_render_position &&
            has_anchor_render_toolbar &&
            has_anchor_render_action &&
            has_anchor_render_page_heading &&
            has_anchor_render_page_context_badges &&
            has_anchor_render_header_tabs &&
            has_anchor_render_header_subtitle &&
            has_anchor_render_properties_footer &&
            has_anchor_render_inspector_band_badges &&
            has_anchor_render_viewport_feedback &&
            has_anchor_handle_hot_state &&
            has_anchor_handle_hot_render &&
            has_anchor_handle_tooling &&
            has_dragging_anchor_state &&
            has_dragging_anchor_tooling &&
            has_anchor_handle_drag_behavior &&
            has_anchor_handle_drag_persisted_render &&
            has_anchor_handle_drag_commit_behavior &&
            has_dragging_anchor_render &&
            moved_anchor &&
            resized_anchor &&
            has_selected_position_line &&
            has_selected_radius_line &&
            has_manipulated_anchor_state_view_model_dirty &&
            has_manipulated_anchor_state_view_model_save &&
            has_manipulated_anchor_render_radius &&
            cleared_selection &&
            has_cleared_selection_line &&
            cleared_selected_region_line &&
            cleared_selected_anchor_line &&
            has_cleared_state_view_model_selection &&
            has_cleared_state_view_model_save &&
            has_story_line &&
            has_active_anchor_line)
        ? 0
        : 1;
}
