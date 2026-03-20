#include "game/demo/DemoScene.h"

#include "engine/editor/EditorDocument.h"
#include "game/demo/WorldWorkspaceLayout.h"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <string_view>
#include <unordered_set>
#include <utility>

namespace resonance {
namespace {

constexpr float kEventEmphasisDecayPerUpdate = 0.25F;
constexpr std::size_t kMemoryChainTotal = 3U;
constexpr std::string_view kStageOneMemory = "meadow-swing";
constexpr std::string_view kStageTwoMemory = "ruins-gate";
constexpr std::string_view kStageThreeMemory = "lakeside-reeds";
constexpr std::string_view kBaseStationChainEvent = "echoing_announcement";
constexpr std::string_view kStageTwoStationChainEvent = "platform_convergence";
constexpr std::string_view kStageThreeStationChainEvent = "terminal_refrain";
constexpr float kEditorSelectionRadius = 14.0F;
constexpr float kMinimumEditorAnchorRadius = 4.0F;
constexpr float kEditorViewportFocusPadding = 48.0F;
constexpr float kEditorViewportMinimumFocusSize = 120.0F;

}  // namespace

DemoScene::DemoScene(DemoContentBundle bundle)
    : world_(bundle.regions),
      event_director_(std::move(bundle.events)),
      story_anchors_(std::move(bundle.story_anchors)) {
    audio_director_.register_music_states(bundle.music_states);
    for (const auto& region : bundle.regions) {
        audio_director_.set_region_music(region.id, region.default_music_state);
    }
}

DemoScene::DemoScene(std::vector<RegionData> regions)
    : DemoScene(DemoContentBundle{std::move(regions), {}, {}, {}}) {}

void DemoScene::set_player_position(WorldPosition position) {
    world_.set_player_position(position);
}

void DemoScene::move_player(WorldPosition delta) {
    const WorldPosition current = world_.player_position();
    world_.set_player_position({current.x + delta.x, current.y + delta.y});
}

void DemoScene::update() {
    seconds_since_last_major_event_ += 1.0;
    current_event_emphasis_ = std::max(0.0F, current_event_emphasis_ - kEventEmphasisDecayPerUpdate);
    const auto region_id = world_.current_region_id();
    const bool region_changed = region_id != last_region_id_;

    if (region_changed) {
        current_event_id_.clear();
        current_event_music_state_.clear();
        current_event_mix_profile_ = {};
        last_region_event_id_.clear();
        seconds_since_last_major_event_ = 120.0;
        current_event_emphasis_ = 0.0F;
        last_region_id_ = region_id;
    }

    nearby_story_anchor_id_.clear();
    interaction_prompt_.clear();
    current_story_focus_ = 0.0F;
    audio_director_.clear_story_cue();
    audio_director_.set_story_focus(0.0F);
    audio_director_.set_event_emphasis(0.0F);
    audio_director_.set_event_mix(1.0F, 1.0F);
    audio_director_.enter_region(region_id);

    if (!region_id.empty()) {
        std::unordered_set<std::string> recent_event_ids;
        if (!last_region_event_id_.empty()) {
            recent_event_ids.insert(last_region_event_id_);
        }

        std::unordered_set<std::string> world_tags;
        for (const auto& entry : memory_journal_entries_) {
            world_tags.insert("memory:" + entry.id);
        }

        const auto event = event_director_.pick_event({region_id, world_tags, seconds_since_last_major_event_, recent_event_ids});
        if (event.has_value()) {
            if (event->id != current_event_id_) {
                trace_log_.push("Event: " + event->id);
                current_event_emphasis_ = 1.0F;
            }
            current_event_id_ = event->id;
            current_event_music_state_ = event->requested_music_state;
            current_event_mix_profile_ = event->mix_profile;
            last_region_event_id_ = event->id;
            seconds_since_last_major_event_ = 0.0;
        }

        if (!current_event_music_state_.empty()) {
            audio_director_.request_story_cue(current_event_music_state_);
        }

        if (const auto* anchor = find_nearby_story_anchor(region_id); anchor != nullptr) {
            nearby_story_anchor_id_ = anchor->id;
            interaction_prompt_ = anchor->prompt_text;
            current_story_focus_ = story_anchor_focus(*anchor);
        }

        audio_director_.set_event_mix(current_event_mix_profile_.event_duck, current_event_mix_profile_.ambient_boost);
        audio_director_.set_story_focus(current_story_focus_);
        audio_director_.set_event_emphasis(current_event_emphasis_);
    } else {
        current_event_id_.clear();
        current_event_music_state_.clear();
        current_event_mix_profile_ = {};
        last_region_event_id_.clear();
        current_event_emphasis_ = 0.0F;
    }

    if (journal_is_open_) {
        overlay_lines_ = debug_overlay_.build_journal_lines(
            memory_journal_entries_,
            OverlayNarrativeTelemetry{
                memory_journal_entries_.size(),
                latest_memory_id(),
                memory_chain_stage(),
                kMemoryChainTotal,
                station_chain_event(),
                next_memory_id(),
                next_station_upgrade(),
                visible_events_text(),
                next_event_unlock_text(),
            });
    } else {
        overlay_lines_ = debug_overlay_.build_lines(
            region_id,
            audio_director_.current_music_state(),
            current_event_id_,
            interaction_prompt_,
            active_story_text_,
            nearby_story_anchor_id_,
            active_story_anchor_id_,
            trace_log_,
            OverlayAudioTelemetry{
                current_story_focus_,
                current_event_emphasis_,
                current_event_mix_profile_.event_duck,
                current_event_mix_profile_.ambient_boost,
                current_bgm_gain(),
                current_ambient_gain_multiplier(),
            },
            OverlayNarrativeTelemetry{
                memory_journal_entries_.size(),
                latest_memory_id(),
                memory_chain_stage(),
                kMemoryChainTotal,
                station_chain_event(),
                next_memory_id(),
                next_station_upgrade(),
                visible_events_text(),
                next_event_unlock_text(),
            });
    }

    std::vector<std::string> editor_overlay_lines;
    editor_overlay_lines.push_back(std::string{"Mode: "} + (editor_mode_ ? "Edit" : "Play"));

    if (editor_mode_) {
        editor_overlay_lines.push_back("Workspace: " + editor_workspace_id_);
        editor_overlay_lines.push_back("Workspace Tabs: World | Anchors | Events | Audio");
        editor_overlay_lines.push_back(std::string{"Workspace View: "} + (editor_workspace_id_ == "anchor_workspace" ? "Anchor Canvas" : "World Canvas"));
        {
            std::ostringstream viewport_zoom_line;
            viewport_zoom_line << std::fixed << std::setprecision(2) << "Viewport Zoom: " << editor_viewport_zoom_ << "x";
            std::ostringstream viewport_origin_line;
            viewport_origin_line << std::fixed << std::setprecision(1)
                                 << "Viewport Origin: " << editor_viewport_origin_.x << ", " << editor_viewport_origin_.y;
            editor_overlay_lines.push_back(viewport_zoom_line.str());
            editor_overlay_lines.push_back(viewport_origin_line.str());
            editor_overlay_lines.push_back(std::string{"Viewport Pan: "} + (editor_viewport_pan_active_ ? "active" : "idle"));
        }
        if (editor_workspace_id_ == "anchor_workspace") {
            editor_overlay_lines.push_back("Workspace Cards: Anchor Overview | Anchor Audio | Anchor Status");
            editor_overlay_lines.push_back("Workspace Inspector: anchor_selection | runtime_context | session_status");
        } else {
            editor_overlay_lines.push_back("Workspace Cards: Region Summary | Audio Defaults | Canvas Tools");
            editor_overlay_lines.push_back("Workspace Inspector: region_identity | audio_defaults | runtime_telemetry");
        }
        EditorSelection selection;
        if (!selected_story_anchor_id_.empty()) {
            selection = EditorSelection{EditorSelectionKind::story_anchor, selected_story_anchor_id_};
        } else if (!selected_region_id_.empty()) {
            selection = EditorSelection{EditorSelectionKind::region, selected_region_id_};
        }

        editor_overlay_lines.push_back("Editor Controls: Click select | Drag/WASD move | Wheel/[ ] size | Shift+F frame | 0 zoom | F5 save");
        editor_overlay_lines.push_back(std::string{"Drag State: "} + (editor_drag_active_ ? "preview" : "idle"));
        if (!hovered_gizmo_id_.empty()) {
            editor_overlay_lines.push_back("Hovered Gizmo: " + hovered_gizmo_id_);
        }
        if (selected_region_id_.empty() && selected_story_anchor_id_.empty()) {
            editor_overlay_lines.push_back("Editor Selection: none");
        }
        if (!hovered_region_id_.empty()) {
            editor_overlay_lines.push_back("Hovered Region: " + hovered_region_id_);
        }
        if (!hovered_story_anchor_id_.empty()) {
            editor_overlay_lines.push_back("Hovered Anchor: " + hovered_story_anchor_id_);
        }
        if (!selected_region_id_.empty()) {
            editor_overlay_lines.push_back("Selected Region: " + selected_region_id_);
        }
        if (!selected_story_anchor_id_.empty()) {
            editor_overlay_lines.push_back("Selected Anchor: " + selected_story_anchor_id_);
            if (const auto* selected_anchor = find_story_anchor_by_id(selected_story_anchor_id_); selected_anchor != nullptr) {
                std::ostringstream position_line;
                position_line << std::fixed << std::setprecision(1)
                              << "Selected Position: " << selected_anchor->position.x << ", " << selected_anchor->position.y;
                std::ostringstream radius_line;
                radius_line << std::fixed << std::setprecision(2)
                            << "Selected Radius: " << selected_anchor->activation_radius;
                editor_overlay_lines.push_back(position_line.str());
                editor_overlay_lines.push_back(radius_line.str());
            }
        }

        const auto inspector_state = build_editor_inspector_state(EditorDocument{regions(), story_anchors_}, selection);
        if (inspector_state.region.has_value()) {
            editor_overlay_lines.push_back("Inspector: Region");
            editor_overlay_lines.push_back("Inspector Music: " + inspector_state.region->default_music_state);
            std::ostringstream bounds_line;
            bounds_line << std::fixed << std::setprecision(1)
                        << "Inspector Bounds: "
                        << inspector_state.region->x << ", "
                        << inspector_state.region->y << ", "
                        << inspector_state.region->width << ", "
                        << inspector_state.region->height;
            editor_overlay_lines.push_back(bounds_line.str());
        }
        if (inspector_state.story_anchor.has_value()) {
            editor_overlay_lines.push_back("Inspector: Story Anchor");
            editor_overlay_lines.push_back("Inspector Region: " + inspector_state.story_anchor->region_id);
        }
        editor_overlay_lines.push_back(std::string{"Editor Dirty: "} + (editor_dirty_ ? "yes" : "no"));
        if (!editor_save_status_.empty()) {
            editor_overlay_lines.push_back("Editor Save: " + editor_save_status_);
        }
    }

    overlay_lines_.insert(overlay_lines_.begin(), editor_overlay_lines.begin(), editor_overlay_lines.end());
}

void DemoScene::interact() {
    if (nearby_story_anchor_id_.empty()) {
        return;
    }

    for (const auto& anchor : story_anchors_) {
        if (anchor.id != nearby_story_anchor_id_) {
            continue;
        }

        active_story_anchor_id_ = anchor.id;
        active_story_text_ = anchor.story_text;
        trace_log_.push("Story: " + anchor.id);

        const auto exists = std::find_if(
            memory_journal_entries_.begin(),
            memory_journal_entries_.end(),
            [&anchor](const MemoryJournalEntry& entry) {
                return entry.id == anchor.id;
            });
        if (exists == memory_journal_entries_.end()) {
            memory_journal_entries_.push_back(MemoryJournalEntry{anchor.id, anchor.region_id, anchor.story_text});
        }
        return;
    }
}

bool DemoScene::select_region_at(WorldPosition position) {
    if (!editor_mode_) {
        return false;
    }

    if (hovered_gizmo_id_ == "region_resize" && !selected_region_id_.empty()) {
        hovered_region_id_ = selected_region_id_;
        hovered_story_anchor_id_.clear();
        return true;
    }

    for (const auto& region : regions()) {
        const bool within_x = position.x >= region.bounds.x && position.x < (region.bounds.x + region.bounds.width);
        const bool within_y = position.y >= region.bounds.y && position.y < (region.bounds.y + region.bounds.height);
        if (!within_x || !within_y) {
            continue;
        }

        selected_region_id_ = region.id;
        selected_story_anchor_id_.clear();
        hovered_region_id_ = region.id;
        hovered_story_anchor_id_.clear();
        hovered_gizmo_id_.clear();
        active_gizmo_id_.clear();
        return true;
    }

    selected_region_id_.clear();
    return false;
}

bool DemoScene::clear_editor_hover() {
    const bool changed = !hovered_region_id_.empty() || !hovered_story_anchor_id_.empty() || !hovered_gizmo_id_.empty() ||
        (!editor_drag_active_ && !active_gizmo_id_.empty());
    hovered_region_id_.clear();
    hovered_story_anchor_id_.clear();
    hovered_gizmo_id_.clear();
    if (!editor_drag_active_) {
        active_gizmo_id_.clear();
    }
    return changed;
}

bool DemoScene::set_editor_hover(WorldPosition position) {
    if (!editor_mode_) {
        clear_editor_hover();
        return false;
    }

    std::string next_hovered_anchor_id;
    std::string next_hovered_region_id;
    std::string next_hovered_gizmo_id;
    float best_distance_squared = 0.0F;
    const float selection_radius_squared = kEditorSelectionRadius * kEditorSelectionRadius;

    if (!selected_story_anchor_id_.empty()) {
        if (const auto* selected_anchor = find_story_anchor_by_id(selected_story_anchor_id_); selected_anchor != nullptr) {
            const float handle_dx = position.x - (selected_anchor->position.x + selected_anchor->activation_radius);
            const float handle_dy = position.y - selected_anchor->position.y;
            const float handle_distance_squared = (handle_dx * handle_dx) + (handle_dy * handle_dy);
            if (handle_distance_squared <= selection_radius_squared) {
                next_hovered_anchor_id = selected_anchor->id;
                next_hovered_gizmo_id = "anchor_radius";
            }
        }
    }

    if (next_hovered_gizmo_id.empty() && !selected_region_id_.empty()) {
        for (const auto& region : regions()) {
            if (region.id != selected_region_id_) {
                continue;
            }
            const float handle_dx = position.x - (region.bounds.x + region.bounds.width);
            const float handle_dy = position.y - (region.bounds.y + region.bounds.height);
            if (std::fabs(handle_dx) <= 10.0F && std::fabs(handle_dy) <= 10.0F) {
                next_hovered_region_id = region.id;
                next_hovered_gizmo_id = "region_resize";
            }
            break;
        }
    }

    if (next_hovered_gizmo_id.empty()) {
        for (const auto& anchor : story_anchors_) {
            const float dx = position.x - anchor.position.x;
            const float dy = position.y - anchor.position.y;
            const float distance_squared = (dx * dx) + (dy * dy);
            if (distance_squared > selection_radius_squared) {
                continue;
            }
            if (next_hovered_anchor_id.empty() || distance_squared < best_distance_squared) {
                next_hovered_anchor_id = anchor.id;
                best_distance_squared = distance_squared;
            }
        }

        if (next_hovered_anchor_id.empty()) {
            for (const auto& region : regions()) {
                const bool within_x = position.x >= region.bounds.x && position.x < (region.bounds.x + region.bounds.width);
                const bool within_y = position.y >= region.bounds.y && position.y < (region.bounds.y + region.bounds.height);
                if (!within_x || !within_y) {
                    continue;
                }
                next_hovered_region_id = region.id;
                break;
            }
        }
    }

    const bool changed = hovered_story_anchor_id_ != next_hovered_anchor_id ||
        hovered_region_id_ != next_hovered_region_id ||
        hovered_gizmo_id_ != next_hovered_gizmo_id;
    hovered_story_anchor_id_ = std::move(next_hovered_anchor_id);
    hovered_region_id_ = std::move(next_hovered_region_id);
    hovered_gizmo_id_ = std::move(next_hovered_gizmo_id);
    return changed;
}

void DemoScene::set_editor_drag_active(bool active) {
    const bool was_drag_active = editor_drag_active_;
    editor_drag_active_ = editor_mode_ && active && (!selected_region_id_.empty() || !selected_story_anchor_id_.empty());
    if (editor_drag_active_) {
        if (!was_drag_active) {
            active_gizmo_id_ = hovered_gizmo_id_;
        }
    } else {
        active_gizmo_id_.clear();
        editor_drag_delta_ = WorldPosition{};
    }
}

void DemoScene::set_editor_drag_delta(WorldPosition delta) {
    editor_drag_delta_ = editor_drag_active_ ? delta : WorldPosition{};
}

bool DemoScene::preview_editor_selection(WorldPosition delta) {
    if (!editor_mode_ || !editor_drag_active_ || (selected_region_id_.empty() && selected_story_anchor_id_.empty())) {
        return false;
    }

    editor_drag_delta_.x += delta.x;
    editor_drag_delta_.y += delta.y;
    return true;
}

bool DemoScene::commit_editor_drag() {
    if (!editor_mode_ || !editor_drag_active_) {
        return false;
    }

    const auto preview_delta = editor_drag_delta_;
    const auto preview_gizmo_id = active_gizmo_id_;
    const bool has_preview_delta = preview_delta.x != 0.0F || preview_delta.y != 0.0F;
    editor_drag_active_ = false;
    editor_drag_delta_ = WorldPosition{};
    active_gizmo_id_.clear();

    if (!has_preview_delta) {
        return false;
    }
    if (preview_gizmo_id == "anchor_radius") {
        return adjust_selected_story_anchor_radius(preview_delta.x);
    }
    if (preview_gizmo_id == "region_resize") {
        return resize_selected_region(preview_delta.x, preview_delta.y);
    }
    if (!selected_story_anchor_id_.empty()) {
        return move_selected_story_anchor(preview_delta);
    }
    if (!selected_region_id_.empty()) {
        return move_selected_region(preview_delta);
    }
    return false;
}

bool DemoScene::cancel_editor_drag() {
    const bool had_drag = editor_drag_active_;
    editor_drag_active_ = false;
    editor_drag_delta_ = WorldPosition{};
    active_gizmo_id_.clear();
    return had_drag;
}

bool DemoScene::pan_editor_viewport(WorldPosition delta) {
    if (!editor_mode_ || (delta.x == 0.0F && delta.y == 0.0F)) {
        return false;
    }

    editor_viewport_origin_.x += delta.x;
    editor_viewport_origin_.y += delta.y;
    return true;
}

bool DemoScene::adjust_editor_viewport_zoom(float delta) {
    if (!editor_mode_ || delta == 0.0F) {
        return false;
    }

    const float next_zoom = std::clamp(editor_viewport_zoom_ + delta, 0.5F, 2.5F);
    if (std::fabs(next_zoom - editor_viewport_zoom_) < 0.0001F) {
        return false;
    }

    editor_viewport_zoom_ = next_zoom;
    return true;
}

bool DemoScene::reset_editor_viewport() {
    if (!editor_mode_) {
        return false;
    }

    const bool changed = editor_viewport_origin_.x != 0.0F || editor_viewport_origin_.y != 0.0F ||
        std::fabs(editor_viewport_zoom_ - 1.0F) > 0.0001F || editor_viewport_pan_active_;
    editor_viewport_origin_ = WorldPosition{};
    editor_viewport_zoom_ = 1.0F;
    editor_viewport_pan_active_ = false;
    return changed;
}

bool DemoScene::reset_editor_viewport_zoom() {
    if (!editor_mode_ || std::fabs(editor_viewport_zoom_ - 1.0F) < 0.0001F) {
        return false;
    }

    editor_viewport_zoom_ = 1.0F;
    return true;
}

bool DemoScene::focus_editor_viewport_on_selection() {
    if (!editor_mode_) {
        return false;
    }

    WorldPosition selection_center{};
    float target_width = 0.0F;
    float target_height = 0.0F;

    if (!selected_story_anchor_id_.empty()) {
        const auto* selected_anchor = find_story_anchor_by_id(selected_story_anchor_id_);
        if (selected_anchor == nullptr) {
            return false;
        }
        selection_center = selected_anchor->position;
        const float focus_diameter = std::max(selected_anchor->activation_radius * 2.0F, 0.0F);
        target_width = std::max(focus_diameter + (kEditorViewportFocusPadding * 2.0F), kEditorViewportMinimumFocusSize);
        target_height = std::max(focus_diameter + (kEditorViewportFocusPadding * 2.0F), kEditorViewportMinimumFocusSize);
    } else if (!selected_region_id_.empty()) {
        const auto match = std::find_if(
            regions().begin(),
            regions().end(),
            [this](const RegionData& region) {
                return region.id == selected_region_id_;
            });
        if (match == regions().end()) {
            return false;
        }
        selection_center = WorldPosition{
            match->bounds.x + (match->bounds.width * 0.5F),
            match->bounds.y + (match->bounds.height * 0.5F)};
        target_width = std::max(match->bounds.width + (kEditorViewportFocusPadding * 2.0F), kEditorViewportMinimumFocusSize);
        target_height = std::max(match->bounds.height + (kEditorViewportFocusPadding * 2.0F), kEditorViewportMinimumFocusSize);
    } else {
        return false;
    }

    const auto layout = build_world_workspace_layout(kEditorWindowWidth, kEditorWindowHeight);
    const float next_zoom = std::clamp(
        std::min(layout.viewport_content.w / target_width, layout.viewport_content.h / target_height),
        0.5F,
        2.5F);
    const WorldPosition next_origin{
        selection_center.x - (layout.viewport_content.w * 0.5F / next_zoom),
        selection_center.y - (layout.viewport_content.h * 0.5F / next_zoom)};
    const bool changed = std::fabs(next_origin.x - editor_viewport_origin_.x) > 0.0001F ||
        std::fabs(next_origin.y - editor_viewport_origin_.y) > 0.0001F ||
        std::fabs(next_zoom - editor_viewport_zoom_) > 0.0001F || editor_viewport_pan_active_;
    editor_viewport_origin_ = next_origin;
    editor_viewport_zoom_ = next_zoom;
    editor_viewport_pan_active_ = false;
    return changed;
}

void DemoScene::set_editor_viewport_pan_active(bool active) {
    editor_viewport_pan_active_ = editor_mode_ && active;
}

bool DemoScene::select_story_anchor_at(WorldPosition position) {
    if (!editor_mode_) {
        return false;
    }

    if (hovered_gizmo_id_ == "anchor_radius" && !selected_story_anchor_id_.empty()) {
        hovered_region_id_.clear();
        hovered_story_anchor_id_ = selected_story_anchor_id_;
        return true;
    }

    const StoryAnchorData* best_match = nullptr;
    float best_distance_squared = 0.0F;
    const float selection_radius_squared = kEditorSelectionRadius * kEditorSelectionRadius;

    for (const auto& anchor : story_anchors_) {
        const float dx = position.x - anchor.position.x;
        const float dy = position.y - anchor.position.y;
        const float distance_squared = (dx * dx) + (dy * dy);
        if (distance_squared > selection_radius_squared) {
            continue;
        }

        if (best_match == nullptr || distance_squared < best_distance_squared) {
            best_match = &anchor;
            best_distance_squared = distance_squared;
        }
    }

    if (best_match == nullptr) {
        selected_story_anchor_id_.clear();
        return false;
    }

    selected_region_id_.clear();
    selected_story_anchor_id_ = best_match->id;
    hovered_region_id_.clear();
    hovered_story_anchor_id_ = best_match->id;
    hovered_gizmo_id_.clear();
    active_gizmo_id_.clear();
    return true;
}

bool DemoScene::nudge_editor_selection(WorldPosition delta) {
    if (editor_drag_active_) {
        return preview_editor_selection(delta);
    }
    if (move_selected_story_anchor(delta)) {
        return true;
    }
    return move_selected_region(delta);
}

bool DemoScene::adjust_editor_selection_primary(float delta) {
    const std::string& gizmo_id = active_gizmo_id_.empty() ? hovered_gizmo_id_ : active_gizmo_id_;
    if (gizmo_id == "anchor_radius") {
        return adjust_selected_story_anchor_radius(delta * 2.0F);
    }
    if (gizmo_id == "region_resize") {
        return resize_selected_region(delta * 8.0F, delta * 4.0F);
    }
    if (adjust_selected_story_anchor_radius(delta * 2.0F)) {
        return true;
    }
    return resize_selected_region(delta * 8.0F, delta * 4.0F);
}

bool DemoScene::clear_editor_selection() {
    if (!editor_mode_) {
        return false;
    }

    const bool had_selection = !selected_region_id_.empty() || !selected_story_anchor_id_.empty();
    selected_region_id_.clear();
    selected_story_anchor_id_.clear();
    hovered_gizmo_id_.clear();
    active_gizmo_id_.clear();
    editor_drag_active_ = false;
    editor_drag_delta_ = WorldPosition{};
    return had_selection;
}

bool DemoScene::move_selected_region(WorldPosition delta) {
    if (!editor_mode_ || selected_region_id_.empty()) {
        return false;
    }

    for (auto& region : world_.regions()) {
        if (region.id != selected_region_id_) {
            continue;
        }

        region.bounds.x += delta.x;
        region.bounds.y += delta.y;
        editor_dirty_ = true;
        editor_save_status_ = "modified";
        world_.set_player_position(world_.player_position());
        return true;
    }

    selected_region_id_.clear();
    return false;
}

bool DemoScene::resize_selected_region(float width_delta, float height_delta) {
    if (!editor_mode_ || selected_region_id_.empty()) {
        return false;
    }

    for (auto& region : world_.regions()) {
        if (region.id != selected_region_id_) {
            continue;
        }

        region.bounds.width = std::max(16.0F, region.bounds.width + width_delta);
        region.bounds.height = std::max(16.0F, region.bounds.height + height_delta);
        editor_dirty_ = true;
        editor_save_status_ = "modified";
        world_.set_player_position(world_.player_position());
        return true;
    }

    selected_region_id_.clear();
    return false;
}

bool DemoScene::move_selected_story_anchor(WorldPosition delta) {
    if (!editor_mode_ || selected_story_anchor_id_.empty()) {
        return false;
    }

    for (auto& anchor : story_anchors_) {
        if (anchor.id != selected_story_anchor_id_) {
            continue;
        }

        anchor.position.x += delta.x;
        anchor.position.y += delta.y;
        editor_dirty_ = true;
        editor_save_status_ = "modified";
        return true;
    }

    selected_story_anchor_id_.clear();
    return false;
}

bool DemoScene::adjust_selected_story_anchor_radius(float delta) {
    if (!editor_mode_ || selected_story_anchor_id_.empty()) {
        return false;
    }

    for (auto& anchor : story_anchors_) {
        if (anchor.id != selected_story_anchor_id_) {
            continue;
        }

        anchor.activation_radius = std::max(kMinimumEditorAnchorRadius, anchor.activation_radius + delta);
        editor_dirty_ = true;
        editor_save_status_ = "modified";
        return true;
    }

    selected_story_anchor_id_.clear();
    return false;
}

bool DemoScene::save_editor_document(const std::filesystem::path& data_root) {
    if (!editor_mode_) {
        editor_save_status_ = "blocked";
        trace_log_.push("Editor Save: blocked");
        return false;
    }

    try {
        write_editor_document(EditorDocument{regions(), story_anchors_}, data_root);
        editor_dirty_ = false;
        editor_save_status_ = "saved";
        trace_log_.push("Editor Save: saved");
        return true;
    } catch (const std::exception&) {
        editor_save_status_ = "failed";
        trace_log_.push("Editor Save: failed");
        return false;
    }
}

bool DemoScene::set_editor_workspace(std::string workspace_id) {
    if (!editor_mode_) {
        return false;
    }
    if (workspace_id != "world_workspace" && workspace_id != "anchor_workspace") {
        return false;
    }
    if (editor_workspace_id_ == workspace_id) {
        return true;
    }
    editor_workspace_id_ = std::move(workspace_id);
    return true;
}

void DemoScene::toggle_journal() {
    journal_is_open_ = !journal_is_open_;
}

void DemoScene::toggle_editor_mode() {
    editor_mode_ = !editor_mode_;
    if (editor_mode_) {
        editor_dirty_ = false;
        editor_save_status_.clear();
        editor_workspace_id_ = "world_workspace";
        hovered_region_id_.clear();
        hovered_story_anchor_id_.clear();
        hovered_gizmo_id_.clear();
        active_gizmo_id_.clear();
        editor_drag_active_ = false;
        editor_drag_delta_ = WorldPosition{};
        editor_viewport_origin_ = WorldPosition{};
        editor_viewport_zoom_ = 1.0F;
        editor_viewport_pan_active_ = false;
    } else {
        selected_region_id_.clear();
        selected_story_anchor_id_.clear();
        hovered_region_id_.clear();
        hovered_story_anchor_id_.clear();
        hovered_gizmo_id_.clear();
        active_gizmo_id_.clear();
        editor_drag_active_ = false;
        editor_drag_delta_ = WorldPosition{};
        editor_workspace_id_ = "world_workspace";
        editor_viewport_origin_ = WorldPosition{};
        editor_viewport_zoom_ = 1.0F;
        editor_viewport_pan_active_ = false;
    }
}

std::string DemoScene::current_region_id() const {
    return world_.current_region_id();
}

std::string DemoScene::current_music_state() const {
    return audio_director_.current_music_state();
}

std::string DemoScene::current_event_id() const {
    return current_event_id_;
}

float DemoScene::current_story_focus() const {
    return current_story_focus_;
}

float DemoScene::current_event_emphasis() const {
    return current_event_emphasis_;
}

float DemoScene::current_bgm_gain() const {
    return audio_director_.current_bgm_gain();
}

float DemoScene::current_ambient_gain_multiplier() const {
    return audio_director_.current_ambient_gain_multiplier();
}

EventMixProfile DemoScene::current_event_mix_profile() const {
    return current_event_mix_profile_;
}

std::string DemoScene::current_story_anchor_id() const {
    return active_story_anchor_id_;
}

std::string DemoScene::current_interaction_prompt() const {
    return interaction_prompt_;
}

std::string DemoScene::active_story_text() const {
    return active_story_text_;
}

std::string DemoScene::selected_region_id() const {
    return selected_region_id_;
}

std::string DemoScene::hovered_region_id() const {
    return hovered_region_id_;
}

std::string DemoScene::hovered_story_anchor_id() const {
    return hovered_story_anchor_id_;
}

std::string DemoScene::hovered_gizmo_id() const {
    return hovered_gizmo_id_;
}

std::string DemoScene::active_gizmo_id() const {
    return active_gizmo_id_;
}

std::string DemoScene::editor_workspace_id() const {
    return editor_workspace_id_;
}

bool DemoScene::journal_is_open() const {
    return journal_is_open_;
}

bool DemoScene::editor_mode_active() const {
    return editor_mode_;
}

bool DemoScene::editor_drag_active() const {
    return editor_drag_active_;
}

WorldPosition DemoScene::editor_drag_delta() const {
    return editor_drag_delta_;
}

WorldPosition DemoScene::editor_viewport_origin() const {
    return editor_viewport_origin_;
}

float DemoScene::editor_viewport_zoom() const {
    return editor_viewport_zoom_;
}

bool DemoScene::editor_viewport_pan_active() const {
    return editor_viewport_pan_active_;
}

std::vector<StoryAnchorVisual> DemoScene::story_anchor_visuals() const {
    std::vector<StoryAnchorVisual> visuals;
    visuals.reserve(story_anchors_.size());

    for (const auto& anchor : story_anchors_) {
        visuals.push_back(StoryAnchorVisual{
            anchor.id,
            anchor.position,
            anchor.activation_radius,
            anchor.id == nearby_story_anchor_id_,
            anchor.id == active_story_anchor_id_,
            anchor.id == selected_story_anchor_id_,
            anchor.id == hovered_story_anchor_id_,
        });
    }

    return visuals;
}

std::vector<MemoryJournalEntry> DemoScene::memory_journal_entries() const {
    return memory_journal_entries_;
}

std::vector<std::string> DemoScene::overlay_lines() const {
    return overlay_lines_;
}

WorldWorkspaceState DemoScene::world_workspace_state() const {
    EditorSelection selection;
    if (!selected_story_anchor_id_.empty()) {
        selection = EditorSelection{EditorSelectionKind::story_anchor, selected_story_anchor_id_};
    } else if (!selected_region_id_.empty()) {
        selection = EditorSelection{EditorSelectionKind::region, selected_region_id_};
    }

    return build_world_workspace_state(
        EditorDocument{regions(), story_anchors_},
        selection,
        WorldWorkspaceRuntimeSummary{
            editor_workspace_id_,
            editor_mode_ ? "Edit" : "Play",
            current_region_id(),
            current_music_state(),
            current_event_id(),
            current_story_focus(),
            current_event_emphasis(),
            editor_dirty_,
            editor_save_status_,
            hovered_region_id_,
            hovered_story_anchor_id_,
            editor_drag_active_,
            hovered_gizmo_id_,
            editor_drag_delta_.x,
            editor_drag_delta_.y,
            active_gizmo_id_,
            editor_viewport_origin_.x,
            editor_viewport_origin_.y,
            editor_viewport_zoom_,
            editor_viewport_pan_active_,
        });
}

std::string DemoScene::debug_summary() const {
    std::ostringstream stream;
    stream << "Region=" << (current_region_id().empty() ? "<none>" : current_region_id())
           << " | Music=" << (current_music_state().empty() ? "<none>" : current_music_state())
           << " | Event=" << (current_event_id().empty() ? "<none>" : current_event_id())
           << std::fixed << std::setprecision(2)
           << " | StoryFocus=" << current_story_focus_
           << " | EventEmphasis=" << current_event_emphasis_
           << " | EventDuck=" << current_event_mix_profile_.event_duck
           << " | AmbientBoost=" << current_event_mix_profile_.ambient_boost
           << " | BgmGain=" << current_bgm_gain()
           << " | AmbientGain=" << current_ambient_gain_multiplier()
           << " | Memories=" << memory_journal_entries_.size()
           << " | LatestMemory=" << (latest_memory_id().empty() ? std::string{"<none>"} : latest_memory_id())
           << " | ChainStage=" << memory_chain_stage() << "/" << kMemoryChainTotal
           << " | StationChain=" << station_chain_event()
           << " | NextMemory=" << (next_memory_id().empty() ? std::string{"<complete>"} : next_memory_id())
           << " | NextStationUpgrade=" << (next_station_upgrade().empty() ? std::string{"<complete>"} : next_station_upgrade())
           << " | VisibleEvents=" << visible_events_text()
           << " | NextEventUnlock=" << next_event_unlock_text();
    if (!nearby_story_anchor_id_.empty()) {
        stream << " | NearbyAnchor=" << nearby_story_anchor_id_;
    }
    if (!current_story_anchor_id().empty()) {
        stream << " | StoryAnchor=" << current_story_anchor_id();
    }
    EditorSelection inspector_selection;
    if (!selected_story_anchor_id_.empty()) {
        inspector_selection = EditorSelection{EditorSelectionKind::story_anchor, selected_story_anchor_id_};
    } else if (!selected_region_id_.empty()) {
        inspector_selection = EditorSelection{EditorSelectionKind::region, selected_region_id_};
    }
    const auto inspector_state = build_editor_inspector_state(EditorDocument{regions(), story_anchors_}, inspector_selection);

    if (!selected_region_id_.empty()) {
        stream << " | SelectedRegion=" << selected_region_id_;
    }
    if (!selected_story_anchor_id_.empty()) {
        stream << " | SelectedAnchor=" << selected_story_anchor_id_;
        if (const auto* selected_anchor = find_story_anchor_by_id(selected_story_anchor_id_); selected_anchor != nullptr) {
            stream << std::setprecision(1)
                   << " | SelectedPos=" << selected_anchor->position.x << "," << selected_anchor->position.y
                   << std::setprecision(2)
                   << " | SelectedRadius=" << selected_anchor->activation_radius;
        }
    }
    if (inspector_state.region.has_value()) {
        stream << " | Inspector=region:" << inspector_state.region->id
               << std::setprecision(1)
               << " | InspectorBounds=" << inspector_state.region->x << ","
               << inspector_state.region->y << ","
               << inspector_state.region->width << ","
               << inspector_state.region->height
               << std::setprecision(2);
    }
    if (inspector_state.story_anchor.has_value()) {
        stream << " | Inspector=story_anchor:" << inspector_state.story_anchor->id;
    }
    if (editor_mode_) {
        if (!selected_story_anchor_id_.empty()) {
            stream << " | EditorSelection=story_anchor:" << selected_story_anchor_id_;
        } else if (!selected_region_id_.empty()) {
            stream << " | EditorSelection=region:" << selected_region_id_;
        } else {
            stream << " | EditorSelection=none";
        }
        stream << " | EditorDirty=" << (editor_dirty_ ? "dirty" : "clean");
    }
    if (editor_mode_ && !editor_save_status_.empty()) {
        stream << " | EditorSave=" << editor_save_status_;
    }
    stream << " | Editor=" << (editor_mode_ ? "edit" : "play");
    if (journal_is_open()) {
        stream << " | Journal=open";
    }
    return stream.str();
}

WorldPosition DemoScene::player_position() const {
    return world_.player_position();
}

std::size_t DemoScene::region_count() const {
    return world_.region_count();
}

const std::vector<RegionData>& DemoScene::regions() const {
    return world_.regions();
}

std::string DemoScene::visible_events_text() const {
    const auto ids = event_director_.visible_event_ids(debug_event_context());
    if (ids.empty()) {
        return "<none>";
    }

    std::ostringstream stream;
    for (std::size_t index = 0; index < ids.size(); ++index) {
        if (index > 0U) {
            stream << ",";
        }
        stream << ids[index];
    }
    return stream.str();
}

float DemoScene::story_anchor_focus(const StoryAnchorData& anchor) const {
    if (anchor.activation_radius <= 0.0F) {
        return 0.0F;
    }

    const auto player = world_.player_position();
    const float dx = player.x - anchor.position.x;
    const float dy = player.y - anchor.position.y;
    const float distance = std::sqrt((dx * dx) + (dy * dy));
    const float normalized = 1.0F - (distance / anchor.activation_radius);
    return std::clamp(normalized, 0.0F, 1.0F);
}

std::size_t DemoScene::memory_chain_stage() const {
    bool has_stage_one = false;
    bool has_stage_two = false;
    bool has_stage_three = false;

    for (const auto& entry : memory_journal_entries_) {
        if (entry.id == kStageOneMemory) {
            has_stage_one = true;
        } else if (entry.id == kStageTwoMemory) {
            has_stage_two = true;
        } else if (entry.id == kStageThreeMemory) {
            has_stage_three = true;
        }
    }

    if (!has_stage_one) {
        return 0U;
    }
    if (!has_stage_two) {
        return 1U;
    }
    if (!has_stage_three) {
        return 2U;
    }
    return 3U;
}

std::string DemoScene::next_event_unlock_text() const {
    const auto hint = event_director_.next_locked_event_hint(debug_event_context());
    if (!hint.has_value()) {
        return "<none>";
    }

    std::ostringstream stream;
    stream << hint->event_id << "<-";
    for (std::size_t index = 0; index < hint->missing_world_tags.size(); ++index) {
        if (index > 0U) {
            stream << ",";
        }
        stream << hint->missing_world_tags[index];
    }
    return stream.str();
}

std::string DemoScene::next_memory_id() const {
    switch (memory_chain_stage()) {
        case 0U:
            return std::string{kStageOneMemory};
        case 1U:
            return std::string{kStageTwoMemory};
        case 2U:
            return std::string{kStageThreeMemory};
        default:
            return {};
    }
}

std::string DemoScene::next_station_upgrade() const {
    switch (memory_chain_stage()) {
        case 0U:
        case 1U:
            return std::string{kStageTwoStationChainEvent};
        case 2U:
            return std::string{kStageThreeStationChainEvent};
        default:
            return {};
    }
}

std::string DemoScene::latest_memory_id() const {
    if (memory_journal_entries_.empty()) {
        return {};
    }

    return memory_journal_entries_.back().id;
}

std::string DemoScene::station_chain_event() const {
    switch (memory_chain_stage()) {
        case 3U:
            return std::string{kStageThreeStationChainEvent};
        case 2U:
            return std::string{kStageTwoStationChainEvent};
        default:
            return std::string{kBaseStationChainEvent};
    }
}

EventContext DemoScene::debug_event_context() const {
    std::unordered_set<std::string> world_tags;
    for (const auto& entry : memory_journal_entries_) {
        world_tags.insert("memory:" + entry.id);
    }

    return EventContext{current_region_id(), std::move(world_tags), 120.0, {}};
}

const StoryAnchorData* DemoScene::find_story_anchor_by_id(const std::string& anchor_id) const {
    const auto match = std::find_if(
        story_anchors_.begin(),
        story_anchors_.end(),
        [&anchor_id](const StoryAnchorData& anchor) {
            return anchor.id == anchor_id;
        });
    if (match == story_anchors_.end()) {
        return nullptr;
    }
    return &(*match);
}

const StoryAnchorData* DemoScene::find_nearby_story_anchor(const std::string& region_id) const {
    const auto player = world_.player_position();
    const StoryAnchorData* best_match = nullptr;
    float best_distance_squared = 0.0F;

    for (const auto& anchor : story_anchors_) {
        if (anchor.region_id != region_id) {
            continue;
        }

        const float dx = player.x - anchor.position.x;
        const float dy = player.y - anchor.position.y;
        const float distance_squared = (dx * dx) + (dy * dy);
        const float radius_squared = anchor.activation_radius * anchor.activation_radius;
        if (distance_squared > radius_squared) {
            continue;
        }

        if (best_match == nullptr || distance_squared < best_distance_squared) {
            best_match = &anchor;
            best_distance_squared = distance_squared;
        }
    }

    return best_match;
}

}  // namespace resonance
