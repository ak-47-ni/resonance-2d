#pragma once

#include "engine/world/Trigger.h"

#include <array>
#include <string_view>

namespace resonance {

inline constexpr int kPlayWindowWidth = 640;
inline constexpr int kPlayWindowHeight = 640;
inline constexpr int kEditorWindowWidth = 1440;
inline constexpr int kEditorWindowHeight = 1024;

inline constexpr float kHeaderHeight = 72.0F;
inline constexpr float kWorkspaceNavWidth = 220.0F;
inline constexpr float kWorkspaceMainWidth = 900.0F;
inline constexpr float kPropertiesPanelWidth = 320.0F;
inline constexpr float kWorkspaceColumnHeight = 952.0F;
inline constexpr float kViewportWidth = 640.0F;
inline constexpr float kViewportHeight = 412.0F;
inline constexpr float kBottomDockWidth = 640.0F;
inline constexpr float kBottomDockHeight = 144.0F;
inline constexpr float kBottomCardWidth = 196.0F;
inline constexpr float kBottomCardHeight = 116.0F;
inline constexpr float kInspectorCardWidth = 296.0F;
inline constexpr float kViewportToolbarHeight = 28.0F;

struct WorkspaceRect {
    float x = 0.0F;
    float y = 0.0F;
    float w = 0.0F;
    float h = 0.0F;
};

struct WorkspaceTabLayout {
    std::string_view label;
    WorkspaceRect rect;
    bool active = false;
};

struct WorkspaceBadgeLayout {
    std::string_view label;
    WorkspaceRect rect;
};

struct WorldWorkspaceLayout {
    WorkspaceRect header;
    WorkspaceRect header_title;
    WorkspaceRect header_tabs_strip;
    std::array<WorkspaceTabLayout, 4> header_tabs{};
    WorkspaceRect header_status_strip;
    std::array<WorkspaceBadgeLayout, 3> header_status{};

    WorkspaceRect left_rail;
    WorkspaceRect rail_label;
    std::array<WorkspaceRect, 4> rail_items{};
    WorkspaceRect rail_session_status;

    WorkspaceRect workspace_main;
    WorkspaceRect page_title;
    WorkspaceRect page_subtitle;
    WorkspaceRect secondary_action;
    WorkspaceRect primary_action;
    WorkspaceRect viewport_frame;
    WorkspaceRect viewport_toolbar;
    WorkspaceRect viewport_scope_badge;
    WorkspaceRect viewport_content;
    WorkspaceRect viewport_focus_chip;
    WorkspaceRect viewport_hint_band;
    WorkspaceRect viewport_gizmo_badge;
    WorkspaceRect bottom_dock;
    std::array<WorkspaceRect, 3> bottom_cards{};

    WorkspaceRect properties_panel;
    std::array<WorkspaceRect, 3> inspector_cards{};
    WorkspaceRect properties_focus_band;
    WorkspaceRect properties_footer_band;
};

inline WorkspaceRect make_rect(float x, float y, float w, float h) {
    return WorkspaceRect{x, y, w, h};
}

inline bool workspace_rect_contains(const WorkspaceRect& rect, int x, int y) {
    const float fx = static_cast<float>(x);
    const float fy = static_cast<float>(y);
    return fx >= rect.x && fy >= rect.y && fx <= (rect.x + rect.w) && fy <= (rect.y + rect.h);
}

inline WorldWorkspaceLayout build_world_workspace_layout(int window_width, int window_height) {
    const float width = static_cast<float>(window_width);
    const float height = static_cast<float>(window_height);
    const float column_height = height - kHeaderHeight;
    constexpr float status_gap = 8.0F;
    constexpr float status_right_margin = 20.0F;
    constexpr float save_status_width = 96.0F;
    constexpr float dirty_status_width = 80.0F;
    constexpr float mode_status_width = 84.0F;
    const float mode_status_x = width - status_right_margin - mode_status_width;
    const float dirty_status_x = mode_status_x - status_gap - dirty_status_width;
    const float save_status_x = dirty_status_x - status_gap - save_status_width;

    WorldWorkspaceLayout layout{};
    layout.header = make_rect(0.0F, 0.0F, width, kHeaderHeight);
    layout.header_title = make_rect(20.0F, 14.0F, 236.0F, 44.0F);
    layout.header_tabs_strip = make_rect(460.0F, 12.0F, 304.0F, 36.0F);
    layout.header_tabs = {
        WorkspaceTabLayout{"World", make_rect(476.0F, 16.0F, 56.0F, 24.0F), true},
        WorkspaceTabLayout{"Anchors", make_rect(544.0F, 16.0F, 60.0F, 24.0F), false},
        WorkspaceTabLayout{"Events", make_rect(612.0F, 16.0F, 48.0F, 24.0F), false},
        WorkspaceTabLayout{"Audio", make_rect(666.0F, 16.0F, 48.0F, 24.0F), false},
    };
    layout.header_status_strip = make_rect(save_status_x - 12.0F, 12.0F, (mode_status_x + mode_status_width + 12.0F) - (save_status_x - 12.0F), 36.0F);
    layout.header_status = {
        WorkspaceBadgeLayout{"save:none", make_rect(save_status_x, 16.0F, save_status_width, 24.0F)},
        WorkspaceBadgeLayout{"dirty:false", make_rect(dirty_status_x, 16.0F, dirty_status_width, 24.0F)},
        WorkspaceBadgeLayout{"mode:edit", make_rect(mode_status_x, 16.0F, mode_status_width, 24.0F)},
    };

    layout.left_rail = make_rect(0.0F, kHeaderHeight, kWorkspaceNavWidth, column_height);
    layout.rail_label = make_rect(16.0F, 90.0F, 120.0F, 18.0F);
    for (int index = 0; index < 4; ++index) {
        layout.rail_items[static_cast<std::size_t>(index)] = make_rect(
            16.0F,
            120.0F + (42.0F * static_cast<float>(index)),
            188.0F,
            30.0F);
    }
    layout.rail_session_status = make_rect(16.0F, kHeaderHeight + column_height - 164.0F, 188.0F, 72.0F);

    layout.workspace_main = make_rect(kWorkspaceNavWidth, kHeaderHeight, kWorkspaceMainWidth, column_height);
    layout.page_title = make_rect(240.0F, 92.0F, 300.0F, 18.0F);
    layout.page_subtitle = make_rect(240.0F, 126.0F, 520.0F, 18.0F);
    layout.secondary_action = make_rect(900.0F, 90.0F, 96.0F, 28.0F);
    layout.primary_action = make_rect(1008.0F, 90.0F, 112.0F, 28.0F);
    layout.viewport_frame = make_rect(240.0F, 144.0F, kViewportWidth, kViewportHeight);
    layout.viewport_toolbar = make_rect(256.0F, 158.0F, 608.0F, kViewportToolbarHeight);
    layout.viewport_scope_badge = make_rect(724.0F, 160.0F, 124.0F, 24.0F);
    layout.viewport_content = make_rect(256.0F, 198.0F, 608.0F, 342.0F);
    layout.viewport_focus_chip = make_rect(272.0F, 214.0F, 236.0F, 28.0F);
    layout.viewport_hint_band = make_rect(272.0F, 496.0F, 576.0F, 28.0F);
    layout.viewport_gizmo_badge = make_rect(684.0F, 214.0F, 164.0F, 24.0F);
    layout.bottom_dock = make_rect(240.0F, 572.0F, kBottomDockWidth, kBottomDockHeight);
    layout.bottom_cards = {
        make_rect(256.0F, 586.0F, kBottomCardWidth, kBottomCardHeight),
        make_rect(464.0F, 586.0F, kBottomCardWidth, kBottomCardHeight),
        make_rect(672.0F, 586.0F, kBottomCardWidth, kBottomCardHeight),
    };

    layout.properties_panel = make_rect(width - kPropertiesPanelWidth, kHeaderHeight, kPropertiesPanelWidth, column_height);
    layout.inspector_cards = {
        make_rect(width - 308.0F, 92.0F, kInspectorCardWidth, 96.0F),
        make_rect(width - 308.0F, 204.0F, kInspectorCardWidth, 96.0F),
        make_rect(width - 308.0F, 316.0F, kInspectorCardWidth, 108.0F),
    };
    layout.properties_focus_band = make_rect(width - 308.0F, 78.0F, kInspectorCardWidth, 24.0F);
    layout.properties_footer_band = make_rect(width - 308.0F, kHeaderHeight + column_height - 84.0F, kInspectorCardWidth, 56.0F);

    return layout;
}

inline std::string_view world_workspace_tab_id_at(int screen_x, int screen_y, int window_width, int window_height) {
    const auto layout = build_world_workspace_layout(window_width, window_height);
    if (workspace_rect_contains(layout.header_tabs[0].rect, screen_x, screen_y)) {
        return "world_workspace";
    }
    if (workspace_rect_contains(layout.header_tabs[1].rect, screen_x, screen_y)) {
        return "anchor_workspace";
    }
    return {};
}

inline bool world_workspace_viewport_contains(int screen_x, int screen_y, int window_width, int window_height) {
    const auto layout = build_world_workspace_layout(window_width, window_height);
    return workspace_rect_contains(layout.viewport_content, screen_x, screen_y);
}

inline WorldPosition to_world_workspace_position(int screen_x, int screen_y, bool editor_mode_active, int window_width, int window_height) {
    if (!editor_mode_active) {
        return WorldPosition{static_cast<float>(screen_x), static_cast<float>(screen_y)};
    }

    const auto layout = build_world_workspace_layout(window_width, window_height);
    return WorldPosition{
        static_cast<float>(screen_x) - layout.viewport_content.x,
        static_cast<float>(screen_y) - layout.viewport_content.y,
    };
}

inline WorldPosition to_world_workspace_position(
    int screen_x,
    int screen_y,
    WorldPosition viewport_origin,
    float viewport_zoom,
    int window_width,
    int window_height
) {
    const WorldPosition local = to_world_workspace_position(screen_x, screen_y, true, window_width, window_height);
    const float safe_zoom = viewport_zoom <= 0.0F ? 1.0F : viewport_zoom;
    return WorldPosition{
        viewport_origin.x + (local.x / safe_zoom),
        viewport_origin.y + (local.y / safe_zoom),
    };
}

inline WorldPosition to_world_workspace_screen_position(
    WorldPosition world_position,
    WorldPosition viewport_origin,
    float viewport_zoom,
    int window_width,
    int window_height
) {
    const auto layout = build_world_workspace_layout(window_width, window_height);
    const float safe_zoom = viewport_zoom <= 0.0F ? 1.0F : viewport_zoom;
    return WorldPosition{
        layout.viewport_content.x + ((world_position.x - viewport_origin.x) * safe_zoom),
        layout.viewport_content.y + ((world_position.y - viewport_origin.y) * safe_zoom),
    };
}

}  // namespace resonance
