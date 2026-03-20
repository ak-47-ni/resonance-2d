#pragma once

#include "engine/editor/WorldWorkspaceState.h"

#include <array>
#include <string>
#include <string_view>
#include <vector>

namespace resonance {

struct WorldWorkspaceViewModel {
    std::string save_status_label;
    std::string dirty_label;
    std::string mode_label;
    std::string document_mode_label;
    std::string active_workspace_label;
    std::string selection_summary;
    std::string canvas_meta_label;
    std::array<std::string, 2> session_status_lines{};
    std::array<std::string, 4> event_pool_lines{};
    std::array<std::string, 4> resolved_audio_lines{};
    std::array<std::string, 4> recent_log_lines{};
    std::array<std::string, 3> region_identity_lines{};
    std::array<std::string, 3> audio_defaults_lines{};
    std::array<std::string, 3> runtime_telemetry_lines{};
};

inline bool world_workspace_starts_with(const std::string& text, std::string_view prefix) {
    return text.rfind(prefix.data(), 0) == 0;
}

inline std::string world_workspace_find_line(const std::vector<std::string>& lines, std::string_view prefix) {
    for (const auto& line : lines) {
        if (world_workspace_starts_with(line, prefix)) {
            return line;
        }
    }
    return std::string(prefix) + std::string{"<none>"};
}

inline std::string world_workspace_lower_ascii(std::string text) {
    for (char& ch : text) {
        if (ch >= 'A' && ch <= 'Z') {
            ch = static_cast<char>(ch - 'A' + 'a');
        }
    }
    return text;
}

inline std::string world_workspace_line_value(const std::string& line) {
    const auto separator = line.find(": ");
    if (separator == std::string::npos) {
        return line;
    }

    std::string value = line.substr(separator + 2U);
    if (value == "<none>" || value.empty()) {
        return "none";
    }
    return value;
}

inline const WorldWorkspaceCard* world_workspace_find_card(const WorldWorkspaceState& state, std::string_view id) {
    for (const auto& card : state.primary_cards) {
        if (card.id == id) {
            return &card;
        }
    }
    for (const auto& card : state.bottom_cards) {
        if (card.id == id) {
            return &card;
        }
    }
    return nullptr;
}

inline const WorldWorkspaceSection* world_workspace_find_section(const WorldWorkspaceCard* card, std::string_view id) {
    if (card == nullptr) {
        return nullptr;
    }
    for (const auto& section : card->sections) {
        if (section.id == id) {
            return &section;
        }
    }
    return nullptr;
}

inline std::string world_workspace_find_field(const WorldWorkspaceSection* section, std::string_view label) {
    if (section == nullptr) {
        return std::string(label) + std::string{": <none>"};
    }
    for (const auto& field : section->fields) {
        if (field.label == label) {
            return field.label + std::string{": "} + field.value;
        }
    }
    return std::string(label) + std::string{": <none>"};
}

inline WorldWorkspaceViewModel build_world_workspace_view_model(const WorldWorkspaceState& state) {
    WorldWorkspaceViewModel model{};
    const auto* telemetry_card = world_workspace_find_card(state, kWorldWorkspaceCardTelemetry);
    const auto* telemetry = world_workspace_find_section(telemetry_card, kWorldWorkspaceSectionRuntimeTelemetry);
    const auto* region_card = world_workspace_find_card(state, kWorldWorkspaceCardRegion);
    const auto* region = world_workspace_find_section(region_card, kWorldWorkspaceSectionRegionIdentity);
    const auto* audio_card = world_workspace_find_card(state, kWorldWorkspaceCardAudio);
    const auto* audio = world_workspace_find_section(audio_card, kWorldWorkspaceSectionAudioDefaults);
    const auto* status_card = world_workspace_find_card(state, kWorldWorkspaceCardStatus);
    const auto* status = world_workspace_find_section(status_card, kWorldWorkspaceSectionSaveStatus);
    const auto save_field = world_workspace_find_field(status, "Save");

    model.save_status_label = "save:" + world_workspace_lower_ascii(world_workspace_line_value(save_field));
    model.dirty_label = world_workspace_find_field(status, "Dirty") == "Dirty: dirty" ? "dirty:true" : "dirty:false";
    model.mode_label = "mode:" + world_workspace_lower_ascii(state.mode_label);
    model.document_mode_label = "document_mode: " + world_workspace_lower_ascii(state.mode_label);
    model.active_workspace_label = "active_workspace: " + state.active_workspace_id;
    model.selection_summary = state.selection_summary;
    model.canvas_meta_label = world_workspace_find_field(telemetry, "Current Music");
    model.session_status_lines = {model.document_mode_label, model.active_workspace_label};
    model.event_pool_lines = {
        world_workspace_find_field(telemetry, "Current Region"),
        world_workspace_find_field(telemetry, "Current Event"),
        world_workspace_find_field(telemetry, "Story Focus"),
        world_workspace_find_field(telemetry, "Event Emphasis"),
    };
    model.resolved_audio_lines = {
        world_workspace_find_field(audio, "Default Music"),
        world_workspace_find_field(audio, "Ambient Layers"),
        world_workspace_find_field(telemetry, "Current Music"),
        save_field,
    };
    model.recent_log_lines = {
        std::string{"Selection: "} + state.selection_summary,
        world_workspace_find_field(status, "Dirty"),
        save_field,
        model.mode_label,
    };
    model.region_identity_lines = {
        world_workspace_find_field(region, "Region"),
        world_workspace_find_field(region, "Bounds"),
        world_workspace_find_field(region, "Anchor Count"),
    };
    model.audio_defaults_lines = {
        world_workspace_find_field(audio, "Default Music"),
        world_workspace_find_field(audio, "Ambient Layers"),
        world_workspace_find_field(telemetry, "Current Music"),
    };
    model.runtime_telemetry_lines = {
        world_workspace_find_field(telemetry, "Current Event"),
        world_workspace_find_field(telemetry, "Story Focus"),
        world_workspace_find_field(telemetry, "Event Emphasis"),
    };
    return model;
}

inline std::string world_workspace_selection_summary(const std::vector<std::string>& lines) {
    auto selected_region = world_workspace_find_line(lines, "Selected Region: ");
    if (selected_region.find("<none>") == std::string::npos) {
        return selected_region;
    }

    auto selected_anchor = world_workspace_find_line(lines, "Selected Anchor: ");
    if (selected_anchor.find("<none>") == std::string::npos) {
        return selected_anchor;
    }

    return world_workspace_find_line(lines, "Region: ");
}

inline WorldWorkspaceViewModel build_world_workspace_view_model(const std::vector<std::string>& lines) {
    WorldWorkspaceViewModel model{};
    const auto mode_line = world_workspace_find_line(lines, "Mode: ");
    const bool is_dirty = world_workspace_starts_with(world_workspace_find_line(lines, "Editor Dirty: "), "Editor Dirty: yes");
    const auto save_line = world_workspace_find_line(lines, "Editor Save: ");

    model.save_status_label = "save:" + world_workspace_lower_ascii(world_workspace_line_value(save_line));
    model.dirty_label = is_dirty ? "dirty:true" : "dirty:false";
    model.mode_label = world_workspace_starts_with(mode_line, "Mode: Edit") ? "mode:edit" : "mode:play";
    model.document_mode_label = world_workspace_starts_with(mode_line, "Mode: Edit") ? "document_mode: edit" : "document_mode: play";
    model.active_workspace_label = "active_workspace: world_workspace";
    model.selection_summary = world_workspace_selection_summary(lines);
    model.canvas_meta_label = world_workspace_find_line(lines, "Music: ");

    model.session_status_lines = {
        model.document_mode_label,
        model.active_workspace_label,
    };

    model.event_pool_lines = {
        world_workspace_find_line(lines, "Region: "),
        world_workspace_find_line(lines, "Event: "),
        world_workspace_find_line(lines, "Visible Events: "),
        world_workspace_find_line(lines, "Next Event Unlock: "),
    };

    model.resolved_audio_lines = {
        world_workspace_find_line(lines, "Music: "),
        world_workspace_find_line(lines, "Bgm Gain: "),
        world_workspace_find_line(lines, "Ambient Gain: "),
        world_workspace_find_line(lines, "Event Duck: "),
    };

    model.recent_log_lines = {
        world_workspace_find_line(lines, "Memories: "),
        world_workspace_find_line(lines, "Latest Memory: "),
        world_workspace_find_line(lines, "Next Memory: "),
        world_workspace_find_line(lines, "Next Station Upgrade: "),
    };

    model.region_identity_lines = {
        world_workspace_selection_summary(lines),
        world_workspace_find_line(lines, "Inspector: "),
        world_workspace_find_line(lines, "Inspector Bounds: "),
    };

    model.audio_defaults_lines = {
        world_workspace_find_line(lines, "Music: "),
        world_workspace_find_line(lines, "Ambient Boost: "),
        world_workspace_find_line(lines, "Story Focus: "),
    };

    model.runtime_telemetry_lines = {
        world_workspace_find_line(lines, "Nearby Anchor: "),
        world_workspace_find_line(lines, "Visible Events: "),
        world_workspace_find_line(lines, "Next Event Unlock: "),
    };

    return model;
}

}  // namespace resonance
