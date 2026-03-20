#include "engine/editor/EditorDocument.h"
#include "engine/editor/WorldWorkspaceState.h"

#include <cmath>
#include <filesystem>
#include <optional>
#include <string>

namespace {

std::filesystem::path make_temp_root() {
    const auto root = std::filesystem::temp_directory_path() / "resonance_world_workspace_state_test";
    std::filesystem::remove_all(root);
    std::filesystem::create_directories(root / "regions");
    std::filesystem::create_directories(root / "story");
    return root;
}

void copy_fixture_files(const std::filesystem::path& root) {
    std::filesystem::copy_file(
        "assets/data/regions/regions.json",
        root / "regions" / "regions.json",
        std::filesystem::copy_options::overwrite_existing
    );
    std::filesystem::copy_file(
        "assets/data/story/story_anchors.json",
        root / "story" / "story_anchors.json",
        std::filesystem::copy_options::overwrite_existing
    );
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

std::string format_bounds(float x, float y, float width, float height) {
    return format_scalar(x) + ", " + format_scalar(y) + ", " +
        format_scalar(width) + ", " + format_scalar(height);
}

std::string format_position(float x, float y) {
    return format_scalar(x) + ", " + format_scalar(y);
}

const resonance::WorldWorkspaceCard* find_card(
    const std::vector<resonance::WorldWorkspaceCard>& cards,
    const std::string& id
) {
    for (const auto& card : cards) {
        if (card.id == id) {
            return &card;
        }
    }
    return nullptr;
}

const resonance::WorldWorkspaceCard* find_card(
    const resonance::WorldWorkspaceState& state,
    const std::string& id
) {
    if (const auto* primary = find_card(state.primary_cards, id); primary != nullptr) {
        return primary;
    }
    return find_card(state.bottom_cards, id);
}

const resonance::WorldWorkspaceSection* find_section(
    const resonance::WorldWorkspaceCard& card,
    const std::string& id
) {
    for (const auto& section : card.sections) {
        if (section.id == id) {
            return &section;
        }
    }
    return nullptr;
}

std::optional<std::string> find_field_value(
    const resonance::WorldWorkspaceSection& section,
    const std::string& label
) {
    for (const auto& field : section.fields) {
        if (field.label == label) {
            return field.value;
        }
    }
    return std::nullopt;
}

}  // namespace

int main() {
    const auto root = make_temp_root();
    copy_fixture_files(root);

    auto document = resonance::load_editor_document(root);
    auto region_state = resonance::build_editor_inspector_state(
        document,
        resonance::EditorSelection{resonance::EditorSelectionKind::region, "meadow"}
    );
    auto anchor_state = resonance::build_editor_inspector_state(
        document,
        resonance::EditorSelection{resonance::EditorSelectionKind::story_anchor, "meadow-swing"}
    );

    auto edited_region = *region_state.region;
    edited_region.x -= 12.0F;
    edited_region.width += 43.0F;
    edited_region.height += 19.0F;
    resonance::apply_region_inspector(document, edited_region);

    auto edited_anchor = *anchor_state.story_anchor;
    edited_anchor.x += 22.5F;
    edited_anchor.y += 11.0F;
    edited_anchor.activation_radius += 6.0F;
    edited_anchor.prompt_text = "Press E to edit";
    resonance::apply_story_anchor_inspector(document, edited_anchor);

    auto runtime_summary = resonance::WorldWorkspaceRuntimeSummary{};
    runtime_summary.active_workspace_id = "world_workspace";
    runtime_summary.mode_label = "Edit";
    runtime_summary.current_region_id = "meadow";
    runtime_summary.current_music_state = "explore";
    runtime_summary.current_event_id = "wind_call";
    runtime_summary.story_focus = 0.75F;
    runtime_summary.event_emphasis = 0.25F;
    runtime_summary.dirty = false;
    runtime_summary.save_status = "saved";
    const auto no_selection_workspace = resonance::build_world_workspace_state(
        document,
        resonance::EditorSelection{},
        runtime_summary
    );
    const auto region_workspace = resonance::build_world_workspace_state(
        document,
        resonance::EditorSelection{resonance::EditorSelectionKind::region, "meadow"},
        runtime_summary
    );
    auto anchor_runtime_summary = runtime_summary;
    anchor_runtime_summary.current_event_id = "none";
    anchor_runtime_summary.story_focus = 0.5F;
    anchor_runtime_summary.event_emphasis = 0.0F;
    anchor_runtime_summary.dirty = true;
    anchor_runtime_summary.save_status = "pending";
    const auto anchor_workspace = resonance::build_world_workspace_state(
        document,
        resonance::EditorSelection{resonance::EditorSelectionKind::story_anchor, "meadow-swing"},
        anchor_runtime_summary
    );

    const auto* no_selection_region_card = find_card(no_selection_workspace, std::string{resonance::kWorldWorkspaceCardRegion});
    const auto* no_selection_audio_card = find_card(no_selection_workspace, std::string{resonance::kWorldWorkspaceCardAudio});
    const auto* no_selection_telemetry_card = find_card(no_selection_workspace, std::string{resonance::kWorldWorkspaceCardTelemetry});
    const auto* no_selection_status_card = find_card(no_selection_workspace, std::string{resonance::kWorldWorkspaceCardStatus});
    const auto* no_selection_region_section = no_selection_region_card == nullptr ? nullptr : find_section(*no_selection_region_card, std::string{resonance::kWorldWorkspaceSectionRegionIdentity});
    const auto* no_selection_audio_section = no_selection_audio_card == nullptr ? nullptr : find_section(*no_selection_audio_card, std::string{resonance::kWorldWorkspaceSectionAudioDefaults});
    const auto* no_selection_telemetry_section = no_selection_telemetry_card == nullptr ? nullptr : find_section(*no_selection_telemetry_card, std::string{resonance::kWorldWorkspaceSectionRuntimeTelemetry});
    const auto* no_selection_status_section = no_selection_status_card == nullptr ? nullptr : find_section(*no_selection_status_card, std::string{resonance::kWorldWorkspaceSectionSaveStatus});

    const bool schema_counts_ok = no_selection_workspace.primary_cards.size() == 2U && no_selection_workspace.bottom_cards.size() == 3U;
    const bool no_selection_workspace_ok =
        no_selection_workspace.active_workspace_id == "world_workspace" &&
        no_selection_workspace.mode_label == "Edit" &&
        no_selection_workspace.selection_summary == "No selection" &&
        no_selection_region_section != nullptr &&
        no_selection_audio_section != nullptr &&
        no_selection_telemetry_section != nullptr &&
        no_selection_status_section != nullptr &&
        find_field_value(*no_selection_region_section, "Region") == std::optional<std::string>{"meadow"} &&
        find_field_value(*no_selection_audio_section, "Default Music") == std::optional<std::string>{"explore"} &&
        find_field_value(*no_selection_telemetry_section, "Current Event") == std::optional<std::string>{"wind_call"} &&
        find_field_value(*no_selection_status_section, "Dirty") == std::optional<std::string>{"clean"};

    const auto* region_inspector_card = find_card(region_workspace, std::string{resonance::kWorldWorkspaceCardInspector});
    const auto* region_audio_card = find_card(region_workspace, std::string{resonance::kWorldWorkspaceCardAudio});
    const auto* region_inspector_section = region_inspector_card == nullptr ? nullptr : find_section(*region_inspector_card, std::string{resonance::kWorldWorkspaceSectionRegion});
    const auto* region_audio_section = region_audio_card == nullptr ? nullptr : find_section(*region_audio_card, std::string{resonance::kWorldWorkspaceSectionAudioDefaults});
    const bool region_workspace_ok =
        region_workspace.selection_summary == "Region: meadow" &&
        region_inspector_section != nullptr &&
        region_audio_section != nullptr &&
        find_field_value(*region_inspector_section, "Bounds") == std::optional<std::string>{format_bounds(edited_region.x, edited_region.y, edited_region.width, edited_region.height)} &&
        find_field_value(*region_audio_section, "Ambient Layers") == std::optional<std::string>{"wind, birds"};

    const auto* anchor_inspector_card = find_card(anchor_workspace, std::string{resonance::kWorldWorkspaceCardInspector});
    const auto* anchor_region_card = find_card(anchor_workspace, std::string{resonance::kWorldWorkspaceCardRegion});
    const auto* anchor_telemetry_card = find_card(anchor_workspace, std::string{resonance::kWorldWorkspaceCardTelemetry});
    const auto* anchor_status_card = find_card(anchor_workspace, std::string{resonance::kWorldWorkspaceCardStatus});
    const auto* anchor_story_section = anchor_inspector_card == nullptr ? nullptr : find_section(*anchor_inspector_card, std::string{resonance::kWorldWorkspaceSectionStoryAnchor});
    const auto* anchor_region_section = anchor_region_card == nullptr ? nullptr : find_section(*anchor_region_card, std::string{resonance::kWorldWorkspaceSectionRegionIdentity});
    const auto* anchor_telemetry_section = anchor_telemetry_card == nullptr ? nullptr : find_section(*anchor_telemetry_card, std::string{resonance::kWorldWorkspaceSectionRuntimeTelemetry});
    const auto* anchor_status_section = anchor_status_card == nullptr ? nullptr : find_section(*anchor_status_card, std::string{resonance::kWorldWorkspaceSectionSaveStatus});
    const bool anchor_workspace_ok =
        anchor_workspace.selection_summary == "Story Anchor: meadow-swing" &&
        anchor_story_section != nullptr &&
        anchor_region_section != nullptr &&
        anchor_telemetry_section != nullptr &&
        anchor_status_section != nullptr &&
        find_field_value(*anchor_story_section, "Position") == std::optional<std::string>{format_position(edited_anchor.x, edited_anchor.y)} &&
        find_field_value(*anchor_story_section, "Activation Radius") == std::optional<std::string>{format_scalar(edited_anchor.activation_radius)} &&
        find_field_value(*anchor_story_section, "Prompt") == std::optional<std::string>{"Press E to edit"} &&
        find_field_value(*anchor_region_section, "Region") == std::optional<std::string>{"meadow"} &&
        find_field_value(*anchor_telemetry_section, "Current Event") == std::optional<std::string>{"none"} &&
        find_field_value(*anchor_telemetry_section, "Story Focus") == std::optional<std::string>{"0.50"} &&
        find_field_value(*anchor_status_section, "Save") == std::optional<std::string>{"pending"} &&
        find_field_value(*anchor_status_section, "Dirty") == std::optional<std::string>{"dirty"};

    const auto missing_selection_workspace = resonance::build_world_workspace_state(
        document,
        resonance::EditorSelection{resonance::EditorSelectionKind::region, "missing-region"},
        runtime_summary
    );
    const auto* missing_region_card = find_card(missing_selection_workspace, std::string{resonance::kWorldWorkspaceCardRegion});
    const auto* missing_audio_card = find_card(missing_selection_workspace, std::string{resonance::kWorldWorkspaceCardAudio});
    const auto* missing_region_section = missing_region_card == nullptr ? nullptr : find_section(*missing_region_card, std::string{resonance::kWorldWorkspaceSectionRegionIdentity});
    const auto* missing_audio_section = missing_audio_card == nullptr ? nullptr : find_section(*missing_audio_card, std::string{resonance::kWorldWorkspaceSectionAudioDefaults});
    const bool missing_selection_workspace_ok =
        missing_selection_workspace.selection_summary == "Region: missing-region" &&
        missing_region_section != nullptr &&
        missing_audio_section != nullptr &&
        find_field_value(*missing_region_section, "Region") == std::optional<std::string>{"meadow"} &&
        find_field_value(*missing_audio_section, "Default Music") == std::optional<std::string>{"explore"};

    resonance::WorldWorkspaceRuntimeSummary empty_runtime_summary;
    empty_runtime_summary.active_workspace_id = "world_workspace";
    empty_runtime_summary.mode_label = "Edit";
    const auto empty_runtime_workspace = resonance::build_world_workspace_state(
        document,
        resonance::EditorSelection{},
        empty_runtime_summary
    );
    const auto* empty_runtime_region_card = find_card(empty_runtime_workspace, std::string{resonance::kWorldWorkspaceCardRegion});
    const auto* empty_runtime_region_section = empty_runtime_region_card == nullptr ? nullptr : find_section(*empty_runtime_region_card, std::string{resonance::kWorldWorkspaceSectionRegionIdentity});
    const auto* empty_runtime_audio_card = find_card(empty_runtime_workspace, std::string{resonance::kWorldWorkspaceCardAudio});
    const auto* empty_runtime_audio_section = empty_runtime_audio_card == nullptr ? nullptr : find_section(*empty_runtime_audio_card, std::string{resonance::kWorldWorkspaceSectionAudioDefaults});
    const auto* empty_runtime_status_card = find_card(empty_runtime_workspace, std::string{resonance::kWorldWorkspaceCardStatus});
    const auto* empty_runtime_status_section = empty_runtime_status_card == nullptr ? nullptr : find_section(*empty_runtime_status_card, std::string{resonance::kWorldWorkspaceSectionSaveStatus});
    const auto* empty_runtime_telemetry_card = find_card(empty_runtime_workspace, std::string{resonance::kWorldWorkspaceCardTelemetry});
    const auto* empty_runtime_telemetry_section = empty_runtime_telemetry_card == nullptr ? nullptr : find_section(*empty_runtime_telemetry_card, std::string{resonance::kWorldWorkspaceSectionRuntimeTelemetry});
    const bool empty_runtime_workspace_ok =
        empty_runtime_region_section != nullptr &&
        empty_runtime_audio_section != nullptr &&
        empty_runtime_status_section != nullptr &&
        empty_runtime_telemetry_section != nullptr &&
        find_field_value(*empty_runtime_region_section, "Region") == std::optional<std::string>{"none"} &&
        find_field_value(*empty_runtime_audio_section, "Default Music") == std::optional<std::string>{"none"} &&
        find_field_value(*empty_runtime_status_section, "Save") == std::optional<std::string>{"none"} &&
        find_field_value(*empty_runtime_telemetry_section, "Current Region") == std::optional<std::string>{"none"};

    const auto missing_anchor_workspace = resonance::build_world_workspace_state(
        document,
        resonance::EditorSelection{resonance::EditorSelectionKind::story_anchor, "missing-anchor"},
        runtime_summary
    );
    const auto* missing_anchor_region_card = find_card(missing_anchor_workspace, std::string{resonance::kWorldWorkspaceCardRegion});
    const auto* missing_anchor_region_section = missing_anchor_region_card == nullptr ? nullptr : find_section(*missing_anchor_region_card, std::string{resonance::kWorldWorkspaceSectionRegionIdentity});
    const auto* missing_anchor_audio_card = find_card(missing_anchor_workspace, std::string{resonance::kWorldWorkspaceCardAudio});
    const auto* missing_anchor_audio_section = missing_anchor_audio_card == nullptr ? nullptr : find_section(*missing_anchor_audio_card, std::string{resonance::kWorldWorkspaceSectionAudioDefaults});
    const bool missing_anchor_workspace_ok =
        missing_anchor_workspace.selection_summary == "Story Anchor: missing-anchor" &&
        missing_anchor_region_section != nullptr &&
        missing_anchor_audio_section != nullptr &&
        find_field_value(*missing_anchor_region_section, "Region") == std::optional<std::string>{"meadow"} &&
        find_field_value(*missing_anchor_audio_section, "Default Music") == std::optional<std::string>{"explore"};

    auto viewport_runtime_summary = runtime_summary;
    viewport_runtime_summary.viewport_origin_x = 24.0F;
    viewport_runtime_summary.viewport_origin_y = -16.0F;
    viewport_runtime_summary.viewport_zoom = 1.25F;
    const auto viewport_workspace = resonance::build_world_workspace_state(
        document,
        resonance::EditorSelection{},
        viewport_runtime_summary
    );
    viewport_runtime_summary.viewport_pan_active = true;
    const auto viewport_pan_workspace = resonance::build_world_workspace_state(
        document,
        resonance::EditorSelection{},
        viewport_runtime_summary
    );
    const bool viewport_workspace_ok =
        viewport_workspace.viewport_origin_x == 24.0F &&
        viewport_workspace.viewport_origin_y == -16.0F &&
        viewport_workspace.viewport_zoom == 1.25F &&
        !viewport_workspace.viewport_pan_active;
    const bool viewport_pan_workspace_ok =
        viewport_pan_workspace.viewport_origin_x == 24.0F &&
        viewport_pan_workspace.viewport_origin_y == -16.0F &&
        viewport_pan_workspace.viewport_zoom == 1.25F &&
        viewport_pan_workspace.viewport_pan_active;

    auto failed_runtime_summary = runtime_summary;
    failed_runtime_summary.dirty = true;
    failed_runtime_summary.save_status = "failed";
    const auto failed_status_workspace = resonance::build_world_workspace_state(
        document,
        resonance::EditorSelection{},
        failed_runtime_summary
    );
    const auto* failed_status_card = find_card(failed_status_workspace, std::string{resonance::kWorldWorkspaceCardStatus});
    const auto* failed_status_section = failed_status_card == nullptr ? nullptr : find_section(*failed_status_card, std::string{resonance::kWorldWorkspaceSectionSaveStatus});
    const bool failed_status_workspace_ok =
        failed_status_section != nullptr &&
        find_field_value(*failed_status_section, "Dirty") == std::optional<std::string>{"dirty"} &&
        find_field_value(*failed_status_section, "Save") == std::optional<std::string>{"failed"};

    auto active_drag_runtime_summary = runtime_summary;
    active_drag_runtime_summary.hovered_region_id = "meadow";
    active_drag_runtime_summary.drag_active = true;
    active_drag_runtime_summary.hovered_gizmo_id = "region_resize";
    active_drag_runtime_summary.drag_delta_x = 16.0F;
    active_drag_runtime_summary.drag_delta_y = 12.0F;
    active_drag_runtime_summary.active_gizmo_id = "region_resize";
    const auto active_drag_workspace = resonance::build_world_workspace_state(
        document,
        resonance::EditorSelection{resonance::EditorSelectionKind::region, "meadow"},
        active_drag_runtime_summary
    );
    const bool active_drag_workspace_ok =
        active_drag_workspace.drag_active &&
        active_drag_workspace.hovered_region_id == "meadow" &&
        active_drag_workspace.hovered_gizmo_id == "region_resize" &&
        active_drag_workspace.active_gizmo_id == "region_resize" &&
        active_drag_workspace.drag_delta_x == 16.0F &&
        active_drag_workspace.drag_delta_y == 12.0F;

    std::filesystem::remove_all(root);

    return (schema_counts_ok &&
            no_selection_workspace_ok &&
            region_workspace_ok &&
            anchor_workspace_ok &&
            missing_selection_workspace_ok &&
            empty_runtime_workspace_ok &&
            missing_anchor_workspace_ok &&
            viewport_workspace_ok &&
            viewport_pan_workspace_ok &&
            failed_status_workspace_ok &&
            active_drag_workspace_ok)
        ? 0
        : 1;
}
