#include "engine/core/Application.h"
#include "engine/debug/DebugOverlay.h"
#include "engine/event/EventData.h"
#include "game/demo/DebugRoute.h"
#include "game/demo/DemoScene.h"
#include "game/demo/WorldWorkspaceLayout.h"
#include "game/demo/WorldWorkspaceRenderModel.h"
#include "game/demo/WorldWorkspaceViewModel.h"

#include <algorithm>
#include <array>
#include <cstdlib>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

#if RESONANCE_HAS_SDL
#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#endif

namespace {
#if RESONANCE_HAS_SDL
SDL_FRect to_rect(const resonance::RegionBounds& bounds) {
    return SDL_FRect{bounds.x, bounds.y, bounds.width, bounds.height};
}

SDL_FRect transform_rect(
    SDL_FRect rect,
    const SDL_FRect& viewport_content,
    resonance::WorldPosition viewport_origin,
    float viewport_zoom
) {
    const float safe_zoom = viewport_zoom <= 0.0F ? 1.0F : viewport_zoom;
    rect.x = viewport_content.x + ((rect.x - viewport_origin.x) * safe_zoom);
    rect.y = viewport_content.y + ((rect.y - viewport_origin.y) * safe_zoom);
    rect.w *= safe_zoom;
    rect.h *= safe_zoom;
    return rect;
}

SDL_FPoint transform_point(
    resonance::WorldPosition point,
    const SDL_FRect& viewport_content,
    resonance::WorldPosition viewport_origin,
    float viewport_zoom
) {
    const float safe_zoom = viewport_zoom <= 0.0F ? 1.0F : viewport_zoom;
    return SDL_FPoint{
        viewport_content.x + ((point.x - viewport_origin.x) * safe_zoom),
        viewport_content.y + ((point.y - viewport_origin.y) * safe_zoom),
    };
}

SDL_SystemCursor system_cursor_for_cue(std::string_view cue) {
    if (cue == "cursor:crosshair") {
        return SDL_SYSTEM_CURSOR_CROSSHAIR;
    }
    if (cue == "cursor:pointer") {
        return SDL_SYSTEM_CURSOR_POINTER;
    }
    if (cue == "cursor:move" || cue == "cursor:grabbing") {
        return SDL_SYSTEM_CURSOR_MOVE;
    }
    if (cue == "cursor:nwse-resize") {
        return SDL_SYSTEM_CURSOR_NWSE_RESIZE;
    }
    if (cue == "cursor:ew-resize") {
        return SDL_SYSTEM_CURSOR_EW_RESIZE;
    }
    return SDL_SYSTEM_CURSOR_DEFAULT;
}

void apply_system_cursor(std::string_view cue) {
    static std::array<SDL_Cursor*, SDL_SYSTEM_CURSOR_COUNT> cursors{};
    const auto system_cursor = system_cursor_for_cue(cue);
    const auto index = static_cast<std::size_t>(system_cursor);
    if (cursors[index] == nullptr) {
        cursors[index] = SDL_CreateSystemCursor(system_cursor);
    }
    if (cursors[index] != nullptr) {
        SDL_SetCursor(cursors[index]);
    }
}

void draw_card(SDL_Renderer* renderer, const SDL_FRect& rect, SDL_Color fill, SDL_Color stroke) {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, fill.r, fill.g, fill.b, fill.a);
    SDL_RenderFillRect(renderer, &rect);
    SDL_SetRenderDrawColor(renderer, stroke.r, stroke.g, stroke.b, stroke.a);
    SDL_RenderRect(renderer, &rect);
}

void draw_text_lines(SDL_Renderer* renderer, float x, float y, const std::vector<std::string>& lines) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    float cursor_y = y;
    for (const auto& line : lines) {
        SDL_RenderDebugText(renderer, x, cursor_y, line.c_str());
        cursor_y += 12.0F;
    }
}

float badge_width_for_text(std::string_view text, float min_width, float max_width) {
    const float estimated = 22.0F + (static_cast<float>(text.size()) * 6.5F);
    return std::clamp(estimated, min_width, max_width);
}

SDL_Color viewport_frame_cue_color(std::string_view frame_state, SDL_Color accent_fill, SDL_Color accent_muted) {
    if (frame_state == "frame:navigate_view") {
        return accent_fill;
    }
    if (frame_state == "frame:selection_focus" || frame_state == "frame:edit_preview") {
        return SDL_Color{226, 232, 244, 255};
    }
    if (frame_state == "frame:hover_focus") {
        return accent_muted;
    }
    return SDL_Color{94, 104, 126, 255};
}

void draw_viewport_frame_brackets(SDL_Renderer* renderer, const SDL_FRect& rect, SDL_Color color) {
    constexpr float bracket = 18.0F;
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    SDL_RenderLine(renderer, rect.x, rect.y, rect.x + bracket, rect.y);
    SDL_RenderLine(renderer, rect.x, rect.y, rect.x, rect.y + bracket);

    SDL_RenderLine(renderer, rect.x + rect.w - bracket, rect.y, rect.x + rect.w, rect.y);
    SDL_RenderLine(renderer, rect.x + rect.w, rect.y, rect.x + rect.w, rect.y + bracket);

    SDL_RenderLine(renderer, rect.x, rect.y + rect.h - bracket, rect.x, rect.y + rect.h);
    SDL_RenderLine(renderer, rect.x, rect.y + rect.h, rect.x + bracket, rect.y + rect.h);

    SDL_RenderLine(renderer, rect.x + rect.w - bracket, rect.y + rect.h, rect.x + rect.w, rect.y + rect.h);
    SDL_RenderLine(renderer, rect.x + rect.w, rect.y + rect.h - bracket, rect.x + rect.w, rect.y + rect.h);
}

SDL_FRect to_sdl_rect(const resonance::WorkspaceRect& rect) {
    return SDL_FRect{rect.x, rect.y, rect.w, rect.h};
}

void draw_panel_heading(SDL_Renderer* renderer, const SDL_FRect& rect, const char* title, SDL_Color accent) {
    SDL_FRect heading{rect.x + 1.0F, rect.y + 1.0F, rect.w - 2.0F, 28.0F};
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 24, 27, 34, 255);
    SDL_RenderFillRect(renderer, &heading);
    SDL_SetRenderDrawColor(renderer, accent.r, accent.g, accent.b, accent.a);
    SDL_RenderLine(renderer, heading.x + 16.0F, heading.y + heading.h - 4.0F, heading.x + 112.0F, heading.y + heading.h - 4.0F);
    SDL_RenderDebugText(renderer, heading.x + 16.0F, heading.y + 9.0F, title);
}

SDL_Color workspace_tone_fill(std::string_view tone) {
    if (tone == "anchor") {
        return SDL_Color{255, 107, 53, 255};
    }
    return SDL_Color{0, 212, 170, 255};
}

SDL_Color workspace_tone_text(std::string_view tone) {
    if (tone == "anchor") {
        return SDL_Color{32, 16, 10, 255};
    }
    return SDL_Color{9, 17, 15, 255};
}

SDL_Color workspace_tone_surface(std::string_view tone) {
    if (tone == "anchor") {
        return SDL_Color{46, 31, 24, 255};
    }
    return SDL_Color{16, 39, 35, 255};
}

SDL_Color workspace_tone_stroke(std::string_view tone) {
    if (tone == "anchor") {
        return SDL_Color{140, 86, 58, 255};
    }
    return SDL_Color{0, 140, 118, 255};
}

SDL_Color workspace_tone_muted(std::string_view tone) {
    if (tone == "anchor") {
        return SDL_Color{197, 136, 108, 255};
    }
    return SDL_Color{122, 194, 179, 255};
}

void draw_workspace_tab(SDL_Renderer* renderer, const SDL_FRect& rect, const char* label, bool active, SDL_Color active_fill, SDL_Color active_text) {
    if (active) {
        draw_card(renderer, rect, active_fill, active_fill);
        SDL_SetRenderDrawColor(renderer, active_text.r, active_text.g, active_text.b, 255);
        SDL_RenderDebugText(renderer, rect.x + 12.0F, rect.y + 6.0F, label);
        return;
    }

    SDL_SetRenderDrawColor(renderer, 142, 150, 168, 255);
    SDL_RenderDebugText(renderer, rect.x, rect.y + 6.0F, label);
}

struct BadgeRowItem {
    std::string_view label;
    SDL_Color fill;
    SDL_Color text;
    float min_width = 72.0F;
    float max_width = 104.0F;
};

void draw_workspace_badge(SDL_Renderer* renderer, const SDL_FRect& rect, std::string_view label, SDL_Color fill, SDL_Color text) {
    draw_card(renderer, rect, fill, fill);
    SDL_SetRenderDrawColor(renderer, text.r, text.g, text.b, text.a);
    SDL_RenderDebugText(renderer, rect.x + 14.0F, rect.y + 6.0F, label.data());
}

void draw_badge_row_left(
    SDL_Renderer* renderer,
    float start_x,
    float y,
    float height,
    float gap,
    std::initializer_list<BadgeRowItem> badges
) {
    float cursor_x = start_x;
    for (const auto& badge : badges) {
        const float width = badge_width_for_text(badge.label, badge.min_width, badge.max_width);
        SDL_FRect rect{cursor_x, y, width, height};
        draw_workspace_badge(renderer, rect, badge.label, badge.fill, badge.text);
        cursor_x += width + gap;
    }
}

void draw_badge_row_right(
    SDL_Renderer* renderer,
    float end_x,
    float y,
    float height,
    float gap,
    std::initializer_list<BadgeRowItem> badges
) {
    float cursor_right = end_x;
    const auto* begin = badges.begin();
    const auto* item = badges.end();
    while (item != begin) {
        --item;
        const float width = badge_width_for_text(item->label, item->min_width, item->max_width);
        SDL_FRect rect{cursor_right - width, y, width, height};
        draw_workspace_badge(renderer, rect, item->label, item->fill, item->text);
        cursor_right = rect.x - gap;
    }
}

void draw_viewport_info_chip(
    SDL_Renderer* renderer,
    const SDL_FRect& rect,
    const char* title,
    const char* detail,
    SDL_Color fill,
    SDL_Color stroke,
    SDL_Color title_color,
    SDL_Color detail_color
) {
    draw_card(renderer, rect, fill, stroke);
    SDL_SetRenderDrawColor(renderer, title_color.r, title_color.g, title_color.b, title_color.a);
    SDL_RenderDebugText(renderer, rect.x + 12.0F, rect.y + 6.0F, title);
    SDL_SetRenderDrawColor(renderer, detail_color.r, detail_color.g, detail_color.b, detail_color.a);
    SDL_RenderDebugText(renderer, rect.x + 110.0F, rect.y + 6.0F, detail);
}

void draw_viewport_hint_band(
    SDL_Renderer* renderer,
    const SDL_FRect& rect,
    const char* line_one,
    const char* line_two,
    SDL_Color fill,
    SDL_Color stroke,
    SDL_Color text_color
) {
    draw_card(renderer, rect, fill, stroke);
    SDL_SetRenderDrawColor(renderer, text_color.r, text_color.g, text_color.b, text_color.a);
    SDL_RenderDebugText(renderer, rect.x + 12.0F, rect.y + 6.0F, line_one);
    SDL_RenderDebugText(renderer, rect.x + 278.0F, rect.y + 6.0F, line_two);
}

SDL_Color workspace_save_fill(std::string_view label) {
    if (label == "save:saved") {
        return SDL_Color{0, 212, 170, 255};
    }
    if (label == "save:modified" || label == "save:pending") {
        return SDL_Color{255, 208, 120, 255};
    }
    if (label == "save:blocked" || label == "save:failed") {
        return SDL_Color{255, 120, 120, 255};
    }
    return SDL_Color{42, 47, 58, 255};
}

SDL_Color workspace_save_text(std::string_view label) {
    if (label == "save:saved") {
        return SDL_Color{9, 17, 15, 255};
    }
    if (label == "save:modified" || label == "save:pending") {
        return SDL_Color{37, 24, 7, 255};
    }
    if (label == "save:blocked" || label == "save:failed") {
        return SDL_Color{36, 10, 10, 255};
    }
    return SDL_Color{215, 220, 232, 255};
}

void draw_shell_divider(SDL_Renderer* renderer, float x, float y, float h) {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 43, 49, 61, 255);
    SDL_RenderLine(renderer, x, y, x, y + h);
}

void draw_world_grid(
    SDL_Renderer* renderer,
    const SDL_FRect& rect,
    resonance::WorldPosition viewport_origin,
    float viewport_zoom
) {
    const float safe_zoom = viewport_zoom <= 0.0F ? 1.0F : viewport_zoom;
    const float spacing = 64.0F * safe_zoom;
    const float start_x_base = rect.x - std::fmod(viewport_origin.x * safe_zoom, spacing);
    const float start_y_base = rect.y - std::fmod(viewport_origin.y * safe_zoom, spacing);
    float start_x = start_x_base;
    float start_y = start_y_base;
    if (start_x > rect.x) {
        start_x -= spacing;
    }
    if (start_y > rect.y) {
        start_y -= spacing;
    }

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 46, 52, 64, 120);
    for (float x = start_x; x < rect.x + rect.w; x += spacing) {
        SDL_RenderLine(renderer, x, rect.y, x, rect.y + rect.h);
    }
    for (float y = start_y; y < rect.y + rect.h; y += spacing) {
        SDL_RenderLine(renderer, rect.x, y, rect.x + rect.w, y);
    }
    SDL_SetRenderDrawColor(renderer, 66, 74, 92, 180);
    SDL_RenderRect(renderer, &rect);
}

void draw_world_regions(
    SDL_Renderer* renderer,
    const resonance::DemoScene& scene,
    const SDL_FRect& viewport_content,
    resonance::WorldPosition viewport_origin,
    float viewport_zoom
) {
    const float safe_zoom = viewport_zoom <= 0.0F ? 1.0F : viewport_zoom;
    for (const auto& region : scene.regions()) {
        SDL_FRect rect = transform_rect(to_rect(region.bounds), viewport_content, viewport_origin, safe_zoom);
        if (region.id == "meadow") {
            SDL_SetRenderDrawColor(renderer, 38, 102, 91, 255);
        } else if (region.id == "ruins") {
            SDL_SetRenderDrawColor(renderer, 116, 76, 56, 255);
        } else if (region.id == "lakeside") {
            SDL_SetRenderDrawColor(renderer, 63, 98, 124, 255);
        } else {
            SDL_SetRenderDrawColor(renderer, 69, 79, 112, 255);
        }
        SDL_RenderFillRect(renderer, &rect);

        if (region.id == scene.current_region_id()) {
            SDL_SetRenderDrawColor(renderer, 255, 208, 120, 255);
            SDL_RenderRect(renderer, &rect);
        }
        if (scene.hovered_region_id() == region.id && scene.selected_region_id() != region.id) {
            SDL_SetRenderDrawColor(renderer, 160, 220, 255, 220);
            SDL_RenderRect(renderer, &rect);
        }
        if (scene.selected_region_id() == region.id) {
            if (scene.editor_drag_active()) {
                const auto drag_delta = scene.editor_drag_delta();
                SDL_FRect ghost_rect = rect;
                if (scene.active_gizmo_id() == "region_resize") {
                    ghost_rect.w = std::max(16.0F * safe_zoom, ghost_rect.w + (drag_delta.x * safe_zoom));
                    ghost_rect.h = std::max(16.0F * safe_zoom, ghost_rect.h + (drag_delta.y * safe_zoom));
                } else {
                    ghost_rect.x += drag_delta.x * safe_zoom;
                    ghost_rect.y += drag_delta.y * safe_zoom;
                }
                SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
                SDL_SetRenderDrawColor(renderer, 255, 120, 220, 96);
                SDL_RenderRect(renderer, &ghost_rect);
            }
            SDL_SetRenderDrawColor(renderer, 255, 120, 220, 255);
            SDL_RenderRect(renderer, &rect);
            SDL_FRect handle{rect.x + rect.w - 8.0F, rect.y + rect.h - 8.0F, 8.0F, 8.0F};
            const bool region_handle_active = scene.hovered_gizmo_id() == "region_resize" || scene.active_gizmo_id() == "region_resize";
            SDL_SetRenderDrawColor(renderer,
                255,
                region_handle_active ? 208 : 120,
                region_handle_active ? 120 : 220,
                255);
            SDL_RenderFillRect(renderer, &handle);
        }
    }
}

void draw_story_anchor_markers(
    SDL_Renderer* renderer,
    const resonance::DemoScene& scene,
    const SDL_FRect& viewport_content,
    resonance::WorldPosition viewport_origin,
    float viewport_zoom
) {
    const float safe_zoom = viewport_zoom <= 0.0F ? 1.0F : viewport_zoom;
    for (const auto& visual : scene.story_anchor_visuals()) {
        const SDL_FPoint center = transform_point(visual.position, viewport_content, viewport_origin, safe_zoom);
        SDL_FRect outer{center.x - 4.0F, center.y - 4.0F, 8.0F, 8.0F};
        if (visual.is_nearby) {
            SDL_SetRenderDrawColor(renderer, 255, 208, 120, 255);
        } else {
            SDL_SetRenderDrawColor(renderer, 80, 200, 220, 220);
        }
        SDL_RenderFillRect(renderer, &outer);

        if (visual.is_hovered && !visual.is_selected) {
            SDL_FRect hover{center.x - 7.0F, center.y - 7.0F, 14.0F, 14.0F};
            SDL_SetRenderDrawColor(renderer, 160, 220, 255, 220);
            SDL_RenderRect(renderer, &hover);
        }

        if (visual.is_selected) {
            if (scene.editor_drag_active()) {
                const auto drag_delta = scene.editor_drag_delta();
                if (scene.active_gizmo_id() == "anchor_radius") {
                    const float ghost_radius = std::max(4.0F, visual.activation_radius + drag_delta.x);
                    SDL_FRect ghost_radius_rect{
                        center.x - (ghost_radius * safe_zoom),
                        center.y - (ghost_radius * safe_zoom),
                        ghost_radius * 2.0F * safe_zoom,
                        ghost_radius * 2.0F * safe_zoom,
                    };
                    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
                    SDL_SetRenderDrawColor(renderer, 255, 120, 220, 96);
                    SDL_RenderRect(renderer, &ghost_radius_rect);
                } else {
                    const SDL_FPoint ghost_center = transform_point(
                        resonance::WorldPosition{visual.position.x + drag_delta.x, visual.position.y + drag_delta.y},
                        viewport_content,
                        viewport_origin,
                        safe_zoom);
                    SDL_FRect ghost_marker{ghost_center.x - 4.0F, ghost_center.y - 4.0F, 8.0F, 8.0F};
                    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
                    SDL_SetRenderDrawColor(renderer, 255, 120, 220, 96);
                    SDL_RenderRect(renderer, &ghost_marker);
                }
            }
            SDL_FRect selection_radius{
                center.x - (visual.activation_radius * safe_zoom),
                center.y - (visual.activation_radius * safe_zoom),
                visual.activation_radius * 2.0F * safe_zoom,
                visual.activation_radius * 2.0F * safe_zoom,
            };
            SDL_SetRenderDrawColor(renderer, 255, 120, 220, 200);
            SDL_RenderRect(renderer, &selection_radius);

            SDL_FRect selection{center.x - 8.0F, center.y - 8.0F, 16.0F, 16.0F};
            SDL_SetRenderDrawColor(renderer, 255, 120, 220, 255);
            SDL_RenderRect(renderer, &selection);

            const SDL_FPoint handle_center = transform_point(
                resonance::WorldPosition{visual.position.x + visual.activation_radius, visual.position.y},
                viewport_content,
                viewport_origin,
                safe_zoom);
            SDL_FRect radius_handle{handle_center.x - 5.0F, handle_center.y - 5.0F, 10.0F, 10.0F};
            const bool anchor_handle_active = scene.hovered_gizmo_id() == "anchor_radius" || scene.active_gizmo_id() == "anchor_radius";
            SDL_SetRenderDrawColor(renderer,
                255,
                anchor_handle_active ? 208 : 120,
                anchor_handle_active ? 120 : 220,
                255);
            SDL_RenderFillRect(renderer, &radius_handle);
        }

        if (visual.is_active) {
            SDL_FRect inner{center.x - 2.0F, center.y - 2.0F, 4.0F, 4.0F};
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderFillRect(renderer, &inner);
        }
    }
}

void draw_player(
    SDL_Renderer* renderer,
    const resonance::DemoScene& scene,
    const SDL_FRect& viewport_content,
    resonance::WorldPosition viewport_origin,
    float viewport_zoom
) {
    const auto player = scene.player_position();
    const SDL_FPoint player_center = transform_point(player, viewport_content, viewport_origin, viewport_zoom);
    SDL_FRect player_rect{player_center.x - 6.0F, player_center.y - 6.0F, 12.0F, 12.0F};
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &player_rect);
}

void draw_selected_region_label(
    SDL_Renderer* renderer,
    const resonance::DemoScene& scene,
    const SDL_FRect& viewport_content,
    SDL_Color fill,
    SDL_Color stroke,
    SDL_Color text_color
) {
    const auto selected_region_id = scene.selected_region_id();
    if (selected_region_id.empty()) {
        return;
    }

    for (const auto& region : scene.regions()) {
        if (region.id != selected_region_id) {
            continue;
        }
        SDL_FRect rect = transform_rect(to_rect(region.bounds), viewport_content, scene.editor_viewport_origin(), scene.editor_viewport_zoom());
        SDL_FRect label_rect{rect.x + 8.0F, rect.y + 8.0F, 156.0F, 24.0F};
        draw_card(renderer, label_rect, fill, stroke);
        SDL_SetRenderDrawColor(renderer, text_color.r, text_color.g, text_color.b, text_color.a);
        const std::string label = std::string{"region // "} + region.id;
        SDL_RenderDebugText(renderer, label_rect.x + 10.0F, label_rect.y + 6.0F, label.c_str());
        break;
    }
}

void draw_selected_anchor_label(
    SDL_Renderer* renderer,
    const resonance::DemoScene& scene,
    const SDL_FRect& viewport_content,
    SDL_Color fill,
    SDL_Color stroke,
    SDL_Color text_color
) {
    for (const auto& visual : scene.story_anchor_visuals()) {
        if (!visual.is_selected) {
            continue;
        }
        const SDL_FPoint anchor_point = transform_point(visual.position, viewport_content, scene.editor_viewport_origin(), scene.editor_viewport_zoom());
        SDL_FRect label_rect{anchor_point.x + 12.0F, anchor_point.y - 28.0F, 208.0F, 24.0F};
        draw_card(renderer, label_rect, fill, stroke);
        SDL_SetRenderDrawColor(renderer, text_color.r, text_color.g, text_color.b, text_color.a);
        const std::string label = visual.id + std::string{" // r:"} + std::to_string(static_cast<int>(visual.activation_radius));
        SDL_RenderDebugText(renderer, label_rect.x + 10.0F, label_rect.y + 6.0F, label.c_str());
        break;
    }
}

void draw_overlay(SDL_Renderer* renderer, const resonance::DemoScene& scene) {
    resonance::DebugOverlay overlay;
    const auto lines = scene.overlay_lines();
    const auto metrics = overlay.measure_lines(lines);

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180);
    SDL_FRect panel{8.0F, 8.0F, metrics.width, metrics.height};
    SDL_RenderFillRect(renderer, &panel);

    draw_text_lines(renderer, 16.0F, 16.0F, lines);
}

void draw_world_workspace(SDL_Renderer* renderer, const resonance::DemoScene& scene, int window_width, int window_height) {
    const auto layout = resonance::build_world_workspace_layout(window_width, window_height);
    const auto workspace_state = scene.world_workspace_state();
    const auto view_model = resonance::build_world_workspace_view_model(workspace_state);
    const auto render_model = resonance::build_world_workspace_render_model(workspace_state, view_model);
    apply_system_cursor(render_model.toolbar_tool_cursor);

    const SDL_FRect header = to_sdl_rect(layout.header);
    const SDL_FRect header_title = to_sdl_rect(layout.header_title);
    const SDL_FRect left_rail = to_sdl_rect(layout.left_rail);
    const SDL_FRect workspace_main = to_sdl_rect(layout.workspace_main);
    const SDL_FRect properties_panel = to_sdl_rect(layout.properties_panel);
    const SDL_FRect header_tabs_strip = to_sdl_rect(layout.header_tabs_strip);
    const SDL_FRect header_status_strip = to_sdl_rect(layout.header_status_strip);
    const SDL_FRect viewport_frame = to_sdl_rect(layout.viewport_frame);
    const SDL_FRect viewport_toolbar = to_sdl_rect(layout.viewport_toolbar);
    const SDL_FRect viewport_scope_badge = to_sdl_rect(layout.viewport_scope_badge);
    const SDL_FRect viewport_content = to_sdl_rect(layout.viewport_content);
    const SDL_FRect viewport_focus_chip = to_sdl_rect(layout.viewport_focus_chip);
    const SDL_FRect viewport_hint_band = to_sdl_rect(layout.viewport_hint_band);
    const SDL_FRect viewport_gizmo_badge = to_sdl_rect(layout.viewport_gizmo_badge);
    const SDL_FRect bottom_dock = to_sdl_rect(layout.bottom_dock);
    const SDL_FRect properties_focus_band = to_sdl_rect(layout.properties_focus_band);
    const SDL_FRect properties_footer_band = to_sdl_rect(layout.properties_footer_band);
    const SDL_Color accent_fill = workspace_tone_fill(render_model.workspace_tone);
    const SDL_Color accent_text = workspace_tone_text(render_model.workspace_tone);
    const SDL_Color accent_surface = workspace_tone_surface(render_model.workspace_tone);
    const SDL_Color accent_stroke = workspace_tone_stroke(render_model.workspace_tone);
    const SDL_Color accent_muted = workspace_tone_muted(render_model.workspace_tone);

    SDL_SetRenderDrawColor(renderer, 21, 23, 29, 255);
    SDL_RenderClear(renderer);

    draw_card(renderer, header, SDL_Color{28, 31, 39, 255}, SDL_Color{35, 40, 51, 255});
    draw_card(renderer, left_rail, SDL_Color{25, 28, 36, 255}, SDL_Color{25, 28, 36, 255});
    draw_card(renderer, workspace_main, SDL_Color{23, 26, 34, 255}, SDL_Color{23, 26, 34, 255});
    draw_card(renderer, properties_panel, SDL_Color{25, 28, 36, 255}, SDL_Color{25, 28, 36, 255});
    draw_card(renderer, header_tabs_strip, SDL_Color{24, 27, 34, 255}, SDL_Color{40, 46, 58, 255});
    draw_card(renderer, header_status_strip, SDL_Color{24, 27, 34, 255}, SDL_Color{40, 46, 58, 255});
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 43, 49, 61, 255);
    SDL_RenderLine(renderer, 0.0F, header.h - 1.0F, static_cast<float>(window_width), header.h - 1.0F);
    draw_shell_divider(renderer, left_rail.x + left_rail.w, left_rail.y + 20.0F, left_rail.h - 40.0F);
    draw_shell_divider(renderer, properties_panel.x, properties_panel.y + 20.0F, properties_panel.h - 40.0F);

    SDL_SetRenderDrawColor(renderer, 236, 240, 248, 255);
    SDL_RenderDebugText(renderer, header_title.x, header_title.y + 4.0F, "resonance-2d // editor");
    SDL_SetRenderDrawColor(renderer, 117, 129, 152, 255);
    SDL_RenderDebugText(renderer, header_title.x, header_title.y + 30.0F, render_model.header_subtitle.c_str());

    for (std::size_t index = 0; index < layout.header_tabs.size(); ++index) {
        draw_workspace_tab(
            renderer,
            to_sdl_rect(layout.header_tabs[index].rect),
            render_model.header_tabs[index].label.c_str(),
            render_model.header_tabs[index].active,
            accent_fill,
            accent_text);
    }
    SDL_SetRenderDrawColor(renderer, accent_muted.r, accent_muted.g, accent_muted.b, 255);
    SDL_RenderLine(renderer, header_tabs_strip.x + 12.0F, header_tabs_strip.y + header_tabs_strip.h - 4.0F, header_tabs_strip.x + header_tabs_strip.w - 12.0F, header_tabs_strip.y + header_tabs_strip.h - 4.0F);

    draw_badge_row_right(renderer,
        header_status_strip.x + header_status_strip.w - 12.0F,
        header_status_strip.y + 4.0F,
        24.0F,
        8.0F,
        {
            BadgeRowItem{render_model.header_status_badges[0], workspace_save_fill(render_model.header_status_badges[0]), workspace_save_text(render_model.header_status_badges[0])},
            BadgeRowItem{render_model.header_status_badges[1],
                render_model.header_status_badges[1] == "dirty:true" ? SDL_Color{255, 208, 120, 255} : SDL_Color{42, 47, 58, 255},
                render_model.header_status_badges[1] == "dirty:true" ? SDL_Color{37, 24, 7, 255} : SDL_Color{215, 220, 232, 255}},
            BadgeRowItem{render_model.header_status_badges[2], SDL_Color{255, 107, 53, 255}, SDL_Color{20, 14, 11, 255}},
        });

    SDL_SetRenderDrawColor(renderer, 111, 121, 142, 255);
    SDL_RenderDebugText(renderer, layout.rail_label.x, layout.rail_label.y, render_model.rail_label.c_str());
    for (std::size_t index = 0; index < layout.rail_items.size(); ++index) {
        const SDL_FRect rail_item = to_sdl_rect(layout.rail_items[index]);
        const bool active = render_model.rail_items[index].active;
        draw_card(renderer,
            rail_item,
            active ? accent_surface : SDL_Color{28, 31, 39, 255},
            active ? accent_stroke : SDL_Color{28, 31, 39, 255});
        SDL_SetRenderDrawColor(renderer,
            active ? accent_fill.r : 142,
            active ? accent_fill.g : 150,
            active ? accent_fill.b : 168,
            255);
        SDL_RenderDebugText(renderer, rail_item.x + 12.0F, rail_item.y + 9.0F, render_model.rail_items[index].label.c_str());
    }
    draw_card(renderer, to_sdl_rect(layout.rail_session_status), SDL_Color{33, 38, 47, 255}, SDL_Color{44, 50, 63, 255});
    SDL_SetRenderDrawColor(renderer, 236, 240, 248, 255);
    SDL_RenderDebugText(renderer, layout.rail_session_status.x + 12.0F, layout.rail_session_status.y + 12.0F, render_model.inspector_cards[2].title.c_str());
    SDL_SetRenderDrawColor(renderer, 174, 184, 206, 255);
    SDL_RenderDebugText(renderer, layout.rail_session_status.x + 12.0F, layout.rail_session_status.y + 34.0F, render_model.inspector_cards[2].lines[0].c_str());
    SDL_RenderDebugText(renderer, layout.rail_session_status.x + 12.0F, layout.rail_session_status.y + 48.0F, render_model.inspector_cards[2].lines[1].c_str());

    SDL_SetRenderDrawColor(renderer, 236, 240, 248, 255);
    SDL_RenderDebugText(renderer, layout.page_title.x, layout.page_title.y, render_model.page_title.c_str());
    SDL_SetRenderDrawColor(renderer, accent_muted.r, accent_muted.g, accent_muted.b, 255);
    SDL_RenderLine(renderer, layout.page_title.x, layout.page_title.y + 22.0F, layout.page_title.x + 132.0F, layout.page_title.y + 22.0F);
    SDL_SetRenderDrawColor(renderer, 126, 136, 156, 255);
    SDL_RenderDebugText(renderer, layout.page_subtitle.x, layout.page_subtitle.y, render_model.page_subtitle.c_str());

    draw_badge_row_left(renderer,
        layout.page_title.x + 336.0F,
        layout.page_title.y - 2.0F,
        24.0F,
        8.0F,
        {
            BadgeRowItem{render_model.page_context_badges[0], accent_surface, accent_text, 86.0F, 118.0F},
            BadgeRowItem{render_model.page_context_badges[1], SDL_Color{33, 38, 47, 255}, SDL_Color{174, 184, 206, 255}, 86.0F, 118.0F},
            BadgeRowItem{render_model.page_context_badges[2], SDL_Color{33, 38, 47, 255}, SDL_Color{174, 184, 206, 255}, 86.0F, 118.0F},
        });

    draw_workspace_badge(renderer, to_sdl_rect(layout.secondary_action), render_model.action_badges[0].c_str(), SDL_Color{42, 47, 58, 255}, SDL_Color{219, 225, 238, 255});
    draw_workspace_badge(renderer, to_sdl_rect(layout.primary_action), render_model.action_badges[1].c_str(), accent_fill, accent_text);

    draw_card(renderer, viewport_frame, SDL_Color{32, 36, 46, 255}, accent_stroke);
    SDL_SetRenderDrawColor(renderer, 184, 194, 216, 255);
    SDL_RenderDebugText(renderer, viewport_frame.x + 16.0F, viewport_frame.y + 14.0F, render_model.viewport_heading.c_str());
    draw_badge_row_right(renderer,
        viewport_frame.x + viewport_frame.w - 16.0F,
        viewport_frame.y + 8.0F,
        20.0F,
        6.0F,
        {
            BadgeRowItem{render_model.viewport_meta_badges[0], accent_surface, accent_text, 78.0F, 96.0F},
            BadgeRowItem{render_model.viewport_meta_badges[1], SDL_Color{33, 38, 47, 255}, SDL_Color{174, 184, 206, 255}, 110.0F, 132.0F},
            BadgeRowItem{render_model.viewport_meta_badges[2],
                render_model.viewport_meta_badges[2] == "pan:active" ? accent_fill : SDL_Color{33, 38, 47, 255},
                render_model.viewport_meta_badges[2] == "pan:active" ? accent_text : SDL_Color{174, 184, 206, 255},
                74.0F,
                92.0F},
        });
    SDL_SetRenderDrawColor(renderer, 94, 104, 126, 255);
    SDL_RenderDebugText(renderer, viewport_frame.x + viewport_frame.w - 184.0F, viewport_frame.y + 30.0F, render_model.viewport_frame_detail.c_str());
    SDL_SetRenderDrawColor(renderer, accent_muted.r, accent_muted.g, accent_muted.b, 255);
    SDL_RenderLine(renderer, viewport_frame.x + 16.0F, viewport_frame.y + 36.0F, viewport_frame.x + 196.0F, viewport_frame.y + 36.0F);
    draw_card(renderer, viewport_toolbar, SDL_Color{24, 27, 34, 255}, SDL_Color{44, 50, 63, 255});
    SDL_SetRenderDrawColor(renderer, 236, 240, 248, 255);
    SDL_RenderDebugText(renderer, viewport_toolbar.x + 12.0F, viewport_toolbar.y + 8.0F, render_model.toolbar_prefix.c_str());
    SDL_SetRenderDrawColor(renderer, 162, 171, 189, 255);
    SDL_RenderDebugText(renderer, viewport_toolbar.x + 110.0F, viewport_toolbar.y + 8.0F, render_model.toolbar_selection.c_str());

    draw_badge_row_right(renderer,
        viewport_toolbar.x + viewport_toolbar.w - 12.0F,
        viewport_toolbar.y + 2.0F,
        24.0F,
        8.0F,
        {
            BadgeRowItem{render_model.toolbar_status, workspace_save_fill(render_model.toolbar_status), workspace_save_text(render_model.toolbar_status)},
            BadgeRowItem{render_model.toolbar_context_badges[0], accent_surface, accent_text, 78.0F, 104.0F},
            BadgeRowItem{render_model.toolbar_context_badges[1], SDL_Color{33, 38, 47, 255}, SDL_Color{219, 225, 238, 255}, 78.0F, 108.0F},
            BadgeRowItem{render_model.toolbar_context_badges[2], SDL_Color{33, 38, 47, 255}, SDL_Color{174, 184, 206, 255}, 78.0F, 108.0F},
        });
    draw_workspace_badge(renderer, viewport_scope_badge, render_model.viewport_badge.c_str(), accent_surface, accent_muted);

    SDL_FRect viewport_inset{viewport_content.x - 6.0F, viewport_content.y - 6.0F, viewport_content.w + 12.0F, viewport_content.h + 12.0F};
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 18, 20, 26, 255);
    SDL_RenderFillRect(renderer, &viewport_inset);
    const SDL_Color frame_cue = viewport_frame_cue_color(
        render_model.viewport_frame_state,
        accent_fill,
        accent_muted);
    SDL_SetRenderDrawColor(renderer, frame_cue.r, frame_cue.g, frame_cue.b, frame_cue.a);
    SDL_RenderRect(renderer, &viewport_inset);
    draw_viewport_frame_brackets(renderer, viewport_inset, frame_cue);
    draw_card(renderer, viewport_content, SDL_Color{20, 23, 29, 255}, SDL_Color{66, 74, 92, 255});
    const SDL_Rect viewport_clip{
        static_cast<int>(viewport_content.x),
        static_cast<int>(viewport_content.y),
        static_cast<int>(viewport_content.w),
        static_cast<int>(viewport_content.h),
    };
    SDL_SetRenderClipRect(renderer, &viewport_clip);
    draw_world_grid(renderer, viewport_content, scene.editor_viewport_origin(), scene.editor_viewport_zoom());
    draw_world_regions(renderer, scene, viewport_content, scene.editor_viewport_origin(), scene.editor_viewport_zoom());
    draw_story_anchor_markers(renderer, scene, viewport_content, scene.editor_viewport_origin(), scene.editor_viewport_zoom());
    draw_player(renderer, scene, viewport_content, scene.editor_viewport_origin(), scene.editor_viewport_zoom());
    SDL_SetRenderClipRect(renderer, nullptr);
    draw_viewport_info_chip(
        renderer,
        viewport_focus_chip,
        render_model.viewport_focus_label.c_str(),
        render_model.viewport_focus_detail.c_str(),
        accent_surface,
        accent_stroke,
        accent_fill,
        SDL_Color{219, 225, 238, 255});
    if (!render_model.viewport_ghost_label.empty()) {
        SDL_SetRenderDrawColor(renderer, 174, 184, 206, 255);
        const std::string ghost_line = render_model.viewport_ghost_label + std::string{" // "} + render_model.viewport_ghost_detail;
        SDL_RenderDebugText(renderer, viewport_focus_chip.x + 4.0F, viewport_focus_chip.y + 34.0F, ghost_line.c_str());
    }
    draw_viewport_hint_band(
        renderer,
        viewport_hint_band,
        render_model.viewport_hint_lines[0].c_str(),
        render_model.viewport_hint_lines[1].c_str(),
        SDL_Color{18, 21, 28, 220},
        SDL_Color{52, 60, 77, 255},
        SDL_Color{174, 184, 206, 255});
    draw_workspace_badge(renderer, viewport_gizmo_badge, render_model.viewport_gizmo_label.c_str(), SDL_Color{24, 27, 34, 235}, accent_muted);
    SDL_SetRenderDrawColor(renderer, 117, 129, 152, 255);
    SDL_RenderDebugText(renderer, viewport_gizmo_badge.x - 4.0F, viewport_gizmo_badge.y + 30.0F, render_model.viewport_gizmo_detail.c_str());
    draw_selected_region_label(renderer, scene, viewport_content, SDL_Color{18, 21, 28, 230}, accent_stroke, accent_fill);
    draw_selected_anchor_label(renderer, scene, viewport_content, SDL_Color{18, 21, 28, 230}, accent_stroke, accent_fill);

    draw_card(renderer, bottom_dock, SDL_Color{29, 33, 42, 255}, SDL_Color{44, 50, 63, 255});
    for (const auto& card : layout.bottom_cards) {
        draw_card(renderer, to_sdl_rect(card), SDL_Color{35, 40, 51, 255}, SDL_Color{52, 60, 77, 255});
    }
    draw_card(renderer, properties_focus_band, SDL_Color{24, 27, 34, 255}, accent_stroke);
    SDL_SetRenderDrawColor(renderer, accent_fill.r, accent_fill.g, accent_fill.b, 255);
    SDL_RenderDebugText(renderer, properties_focus_band.x + 12.0F, properties_focus_band.y + 6.0F, render_model.inspector_focus_label.c_str());
    draw_badge_row_right(renderer,
        properties_focus_band.x + properties_focus_band.w - 12.0F,
        properties_focus_band.y + 2.0F,
        20.0F,
        8.0F,
        {
            BadgeRowItem{render_model.inspector_focus_badges[0], SDL_Color{33, 38, 47, 255}, SDL_Color{174, 184, 206, 255}, 84.0F, 116.0F},
            BadgeRowItem{render_model.inspector_focus_badges[1], SDL_Color{33, 38, 47, 255}, SDL_Color{174, 184, 206, 255}, 84.0F, 116.0F},
        });
    SDL_SetRenderDrawColor(renderer, 174, 184, 206, 255);
    SDL_RenderDebugText(renderer, properties_focus_band.x + 132.0F, properties_focus_band.y + 6.0F, render_model.inspector_focus_detail.c_str());
    for (std::size_t index = 0; index < layout.inspector_cards.size(); ++index) {
        const SDL_Color stroke = static_cast<int>(index) == render_model.inspector_emphasis_index ? accent_stroke : SDL_Color{52, 60, 77, 255};
        draw_card(renderer, to_sdl_rect(layout.inspector_cards[index]), SDL_Color{33, 38, 47, 255}, stroke);
    }

    static constexpr std::array<SDL_Color, 3> kBottomAccents = {
        SDL_Color{255, 107, 53, 255},
        SDL_Color{0, 212, 170, 255},
        SDL_Color{118, 138, 255, 255},
    };
    static constexpr std::array<SDL_Color, 3> kInspectorAccents = {
        SDL_Color{255, 120, 220, 255},
        SDL_Color{255, 208, 120, 255},
        SDL_Color{0, 212, 170, 255},
    };

    for (std::size_t index = 0; index < layout.bottom_cards.size(); ++index) {
        draw_panel_heading(renderer,
            to_sdl_rect(layout.bottom_cards[index]),
            render_model.bottom_cards[index].title.c_str(),
            kBottomAccents[index]);
        draw_text_lines(renderer,
            layout.bottom_cards[index].x + 16.0F,
            layout.bottom_cards[index].y + 42.0F,
            render_model.bottom_cards[index].lines);
    }

    for (std::size_t index = 0; index < layout.inspector_cards.size(); ++index) {
        draw_panel_heading(renderer,
            to_sdl_rect(layout.inspector_cards[index]),
            render_model.inspector_cards[index].title.c_str(),
            kInspectorAccents[index]);
        draw_text_lines(renderer,
            layout.inspector_cards[index].x + 16.0F,
            layout.inspector_cards[index].y + 42.0F,
            render_model.inspector_cards[index].lines);
    }

    SDL_SetRenderDrawColor(renderer, 111, 121, 142, 255);
    SDL_RenderDebugText(renderer, layout.bottom_dock.x + 16.0F, layout.bottom_dock.y + layout.bottom_dock.h - 14.0F, render_model.shell_footer.c_str());
    draw_card(renderer, properties_footer_band, SDL_Color{24, 27, 34, 255}, accent_stroke);
    draw_badge_row_left(renderer,
        properties_footer_band.x + 14.0F,
        properties_footer_band.y + 8.0F,
        20.0F,
        8.0F,
        {
            BadgeRowItem{render_model.properties_footer_badges[0], accent_surface, accent_text, 96.0F, 124.0F},
            BadgeRowItem{render_model.properties_footer_badges[1], SDL_Color{33, 38, 47, 255}, SDL_Color{174, 184, 206, 255}, 96.0F, 124.0F},
        });
    SDL_SetRenderDrawColor(renderer, accent_muted.r, accent_muted.g, accent_muted.b, 255);
    SDL_RenderDebugText(renderer, properties_footer_band.x + 14.0F, properties_footer_band.y + 32.0F, render_model.properties_footer_lines[0].c_str());
    SDL_SetRenderDrawColor(renderer, 111, 121, 142, 255);
    SDL_RenderDebugText(renderer, properties_footer_band.x + 14.0F, properties_footer_band.y + 46.0F, render_model.properties_footer_lines[1].c_str());

    SDL_RenderPresent(renderer);
}

void draw_scene(SDL_Renderer* renderer, const resonance::DemoScene& scene, int window_width, int window_height) {
    if (scene.editor_mode_active()) {
        draw_world_workspace(renderer, scene, window_width, window_height);
        return;
    }

    apply_system_cursor("cursor:default");
    SDL_SetRenderDrawColor(renderer, 18, 18, 24, 255);
    SDL_RenderClear(renderer);
    const SDL_FRect fullscreen_view{0.0F, 0.0F, static_cast<float>(window_width), static_cast<float>(window_height)};
    draw_world_regions(renderer, scene, fullscreen_view, resonance::WorldPosition{}, 1.0F);
    draw_story_anchor_markers(renderer, scene, fullscreen_view, resonance::WorldPosition{}, 1.0F);
    draw_player(renderer, scene, fullscreen_view, resonance::WorldPosition{}, 1.0F);
    draw_overlay(renderer, scene);
    SDL_RenderPresent(renderer);
}

resonance::WorldPosition to_world_position(
    int screen_x,
    int screen_y,
    const resonance::DemoScene& scene,
    int window_width,
    int window_height
) {
    if (!scene.editor_mode_active()) {
        return resonance::WorldPosition{static_cast<float>(screen_x), static_cast<float>(screen_y)};
    }

    return resonance::to_world_workspace_position(
        screen_x,
        screen_y,
        scene.editor_viewport_origin(),
        scene.editor_viewport_zoom(),
        window_width,
        window_height);
}
#endif
}

int main() {
    resonance::Application app;
    const auto bundle = resonance::load_demo_content("assets/data");
#if !RESONANCE_HAS_SDL
    if (const char* route = std::getenv("RESONANCE_DEMO_ROUTE"); route != nullptr && std::string_view{route} == "memory_chain") {
        const auto summaries = resonance::run_memory_chain_debug_route(bundle);
        for (const auto& summary : summaries) {
            std::cout << summary << '\n';
        }
        return (!summaries.empty() && app.is_headless_ready()) ? 0 : 1;
    }

    resonance::DemoScene scene(bundle);
    scene.set_player_position({10.0F, 10.0F});
    scene.update();
    std::cout << scene.debug_summary() << '\n';
    return (!scene.overlay_lines().empty() && app.is_headless_ready()) ? 0 : 1;
#else
    const bool hidden = std::getenv("RESONANCE_DEMO_HIDDEN") != nullptr;
    if (const char* route = std::getenv("RESONANCE_DEMO_ROUTE"); hidden && route != nullptr && std::string_view{route} == "memory_chain") {
        const auto summaries = resonance::run_memory_chain_debug_route(bundle);
        for (const auto& summary : summaries) {
            std::cout << summary << '\n';
        }
        return (!summaries.empty() && app.is_headless_ready()) ? 0 : 1;
    }

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << '\n';
        return 1;
    }

    if (!SDL_InitSubSystem(SDL_INIT_AUDIO)) {
        std::cerr << "SDL audio unavailable: " << SDL_GetError() << '\n';
    }

    const bool auto_close = std::getenv("RESONANCE_DEMO_AUTOCLOSE") != nullptr;
    SDL_WindowFlags window_flags = hidden ? SDL_WINDOW_HIDDEN : 0;

    SDL_Window* window = SDL_CreateWindow("resonance-2d", resonance::kPlayWindowWidth, resonance::kPlayWindowHeight, window_flags);
    if (window == nullptr) {
        std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << '\n';
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    if (renderer == nullptr) {
        std::cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << '\n';
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    std::string final_summary;
    {
        resonance::DemoScene scene(bundle);
        scene.set_player_position({10.0F, 10.0F});
        scene.update();

        bool running = true;
        bool space_pan_modifier = false;
        int frames = 0;
        while (running) {
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_EVENT_QUIT) {
                    running = false;
                } else if (event.type == SDL_EVENT_KEY_DOWN) {
                    switch (event.key.key) {
                        case SDLK_ESCAPE:
                            if (scene.editor_mode_active() && scene.editor_viewport_pan_active()) {
                                scene.set_editor_viewport_pan_active(false);
                                break;
                            }
                            if (scene.editor_mode_active() && scene.editor_drag_active()) {
                                scene.cancel_editor_drag();
                                break;
                            }
                            running = false;
                            break;
                        case SDLK_LEFT:
                        case SDLK_A:
                            if (scene.editor_mode_active() && (event.key.mod & SDL_KMOD_SHIFT) != 0U) {
                                scene.pan_editor_viewport({-24.0F / scene.editor_viewport_zoom(), 0.0F});
                            } else if (!scene.editor_mode_active() || !scene.nudge_editor_selection({-4.0F, 0.0F})) {
                                scene.move_player({-12.0F, 0.0F});
                            }
                            break;
                        case SDLK_RIGHT:
                        case SDLK_D:
                            if (scene.editor_mode_active() && (event.key.mod & SDL_KMOD_SHIFT) != 0U) {
                                scene.pan_editor_viewport({24.0F / scene.editor_viewport_zoom(), 0.0F});
                            } else if (!scene.editor_mode_active() || !scene.nudge_editor_selection({4.0F, 0.0F})) {
                                scene.move_player({12.0F, 0.0F});
                            }
                            break;
                        case SDLK_UP:
                        case SDLK_W:
                            if (scene.editor_mode_active() && (event.key.mod & SDL_KMOD_SHIFT) != 0U) {
                                scene.pan_editor_viewport({0.0F, -24.0F / scene.editor_viewport_zoom()});
                            } else if (!scene.editor_mode_active() || !scene.nudge_editor_selection({0.0F, -4.0F})) {
                                scene.move_player({0.0F, -12.0F});
                            }
                            break;
                        case SDLK_DOWN:
                        case SDLK_S:
                            if (scene.editor_mode_active() && (event.key.mod & SDL_KMOD_SHIFT) != 0U) {
                                scene.pan_editor_viewport({0.0F, 24.0F / scene.editor_viewport_zoom()});
                            } else if (!scene.editor_mode_active() || !scene.nudge_editor_selection({0.0F, 4.0F})) {
                                scene.move_player({0.0F, 12.0F});
                            }
                            break;
                        case SDLK_E:
                            scene.interact();
                            break;
                        case SDLK_J:
                            scene.toggle_journal();
                            break;
                        case SDLK_TAB:
                            scene.toggle_editor_mode();
                            SDL_SetWindowSize(
                                window,
                                scene.editor_mode_active() ? resonance::kEditorWindowWidth : resonance::kPlayWindowWidth,
                                scene.editor_mode_active() ? resonance::kEditorWindowHeight : resonance::kPlayWindowHeight);
                            break;
                        case SDLK_1:
                            if (scene.editor_mode_active()) {
                                scene.set_editor_workspace("world_workspace");
                            }
                            break;
                        case SDLK_2:
                            if (scene.editor_mode_active()) {
                                scene.set_editor_workspace("anchor_workspace");
                            }
                            break;
                        case SDLK_F:
                            if (scene.editor_mode_active()) {
                                if ((event.key.mod & SDL_KMOD_SHIFT) != 0U) {
                                    scene.focus_editor_viewport_on_selection();
                                } else {
                                    scene.reset_editor_viewport();
                                }
                            }
                            break;
                        case SDLK_F5:
                            if (scene.editor_mode_active()) {
                                scene.save_editor_document("assets/data");
                            }
                            break;
                        case SDLK_BACKSPACE:
                        case SDLK_DELETE:
                            if (scene.editor_mode_active()) {
                                scene.clear_editor_selection();
                            }
                            break;
                        case SDLK_LEFTBRACKET:
                            if (scene.editor_mode_active()) {
                                scene.adjust_editor_selection_primary(-2.0F);
                            }
                            break;
                        case SDLK_RIGHTBRACKET:
                            if (scene.editor_mode_active()) {
                                scene.adjust_editor_selection_primary(2.0F);
                            }
                            break;
                        case SDLK_SPACE:
                            if (scene.editor_mode_active()) {
                                space_pan_modifier = true;
                            }
                            break;
                        case SDLK_0:
                        case SDLK_KP_0:
                            if (scene.editor_mode_active()) {
                                scene.reset_editor_viewport_zoom();
                            }
                            break;
                        case SDLK_MINUS:
                        case SDLK_KP_MINUS:
                            if (scene.editor_mode_active()) {
                                scene.adjust_editor_viewport_zoom(-0.25F);
                            }
                            break;
                        case SDLK_EQUALS:
                        case SDLK_PLUS:
                        case SDLK_KP_PLUS:
                            if (scene.editor_mode_active()) {
                                scene.adjust_editor_viewport_zoom(0.25F);
                            }
                            break;
                        default:
                            break;
                    }
                } else if (event.type == SDL_EVENT_KEY_UP) {
                    if (event.key.key == SDLK_SPACE) {
                        space_pan_modifier = false;
                        if (scene.editor_viewport_pan_active()) {
                            scene.set_editor_viewport_pan_active(false);
                        }
                    }
                } else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                    if (scene.editor_mode_active() && event.button.button == SDL_BUTTON_RIGHT) {
                        if (scene.editor_viewport_pan_active()) {
                            scene.set_editor_viewport_pan_active(false);
                        } else if (!scene.cancel_editor_drag()) {
                            scene.clear_editor_selection();
                        }
                    }
                    if (scene.editor_mode_active() && event.button.button == SDL_BUTTON_MIDDLE) {
                        int window_width = 0;
                        int window_height = 0;
                        SDL_GetWindowSize(window, &window_width, &window_height);
                        if (resonance::world_workspace_viewport_contains(
                                static_cast<int>(event.button.x),
                                static_cast<int>(event.button.y),
                                window_width,
                                window_height)) {
                            scene.set_editor_viewport_pan_active(true);
                        }
                    }
                    if (scene.editor_mode_active() && event.button.button == SDL_BUTTON_LEFT) {
                        int window_width = 0;
                        int window_height = 0;
                        SDL_GetWindowSize(window, &window_width, &window_height);
                        const auto workspace_tab = resonance::world_workspace_tab_id_at(
                            static_cast<int>(event.button.x),
                            static_cast<int>(event.button.y),
                            window_width,
                            window_height);
                        if (!workspace_tab.empty()) {
                            scene.set_editor_workspace(std::string{workspace_tab});
                            break;
                        }
                        const bool pointer_in_viewport = resonance::world_workspace_viewport_contains(
                            static_cast<int>(event.button.x),
                            static_cast<int>(event.button.y),
                            window_width,
                            window_height);
                        if (!pointer_in_viewport) {
                            scene.set_editor_viewport_pan_active(false);
                            scene.set_editor_drag_active(false);
                            scene.set_editor_drag_delta({0.0F, 0.0F});
                            scene.clear_editor_hover();
                            break;
                        }
                        if (space_pan_modifier) {
                            scene.set_editor_viewport_pan_active(true);
                            scene.set_editor_drag_active(false);
                            scene.set_editor_drag_delta({0.0F, 0.0F});
                            break;
                        }
                        const resonance::WorldPosition click_position = to_world_position(
                            static_cast<int>(event.button.x),
                            static_cast<int>(event.button.y),
                            scene,
                            window_width,
                            window_height);
                        scene.set_editor_viewport_pan_active(false);
                        scene.set_editor_drag_active(false);
                        scene.set_editor_drag_delta({0.0F, 0.0F});
                        scene.set_editor_hover(click_position);
                        if (!scene.select_story_anchor_at(click_position)) {
                            scene.select_region_at(click_position);
                        }
                    }
                } else if (event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
                    if (scene.editor_mode_active() && event.button.button == SDL_BUTTON_MIDDLE) {
                        scene.set_editor_viewport_pan_active(false);
                    }
                    if (scene.editor_mode_active() && event.button.button == SDL_BUTTON_LEFT) {
                        if (scene.editor_viewport_pan_active()) {
                            scene.set_editor_viewport_pan_active(false);
                        } else if (!scene.commit_editor_drag()) {
                            scene.set_editor_drag_active(false);
                            scene.set_editor_drag_delta({0.0F, 0.0F});
                        }
                    }
                } else if (event.type == SDL_EVENT_MOUSE_MOTION) {
                    if (scene.editor_mode_active() && scene.editor_viewport_pan_active()) {
                        const float zoom = scene.editor_viewport_zoom();
                        const resonance::WorldPosition pan_delta{
                            -static_cast<float>(event.motion.xrel) / zoom,
                            -static_cast<float>(event.motion.yrel) / zoom};
                        scene.pan_editor_viewport(pan_delta);
                    } else if (scene.editor_mode_active()) {
                        int window_width = 0;
                        int window_height = 0;
                        SDL_GetWindowSize(window, &window_width, &window_height);
                        const bool pointer_in_viewport = resonance::world_workspace_viewport_contains(
                            static_cast<int>(event.motion.x),
                            static_cast<int>(event.motion.y),
                            window_width,
                            window_height);
                        if (pointer_in_viewport) {
                            const resonance::WorldPosition hover_position = to_world_position(
                                static_cast<int>(event.motion.x),
                                static_cast<int>(event.motion.y),
                                scene,
                                window_width,
                                window_height);
                            scene.set_editor_hover(hover_position);
                        } else if (!scene.editor_drag_active()) {
                            scene.clear_editor_hover();
                        }
                        if ((event.motion.state & SDL_BUTTON_LMASK) == 0U) {
                            scene.set_editor_drag_delta({0.0F, 0.0F});
                        }
                    }
                    if (scene.editor_mode_active() && !scene.editor_viewport_pan_active() && ((event.motion.state & SDL_BUTTON_LMASK) != 0U)) {
                        int window_width = 0;
                        int window_height = 0;
                        SDL_GetWindowSize(window, &window_width, &window_height);
                        const bool pointer_in_viewport = resonance::world_workspace_viewport_contains(
                            static_cast<int>(event.motion.x),
                            static_cast<int>(event.motion.y),
                            window_width,
                            window_height);
                        if (pointer_in_viewport || scene.editor_drag_active()) {
                            const float zoom = scene.editor_viewport_zoom();
                            const resonance::WorldPosition drag_delta{
                                static_cast<float>(event.motion.xrel) / zoom,
                                static_cast<float>(event.motion.yrel) / zoom};
                            scene.set_editor_drag_active(true);
                            scene.nudge_editor_selection(drag_delta);
                        }
                    }
                } else if (event.type == SDL_EVENT_MOUSE_WHEEL) {
                    if (scene.editor_mode_active()) {
                        int window_width = 0;
                        int window_height = 0;
                        SDL_GetWindowSize(window, &window_width, &window_height);
                        float mouse_x = 0.0F;
                        float mouse_y = 0.0F;
                        SDL_GetMouseState(&mouse_x, &mouse_y);
                        if (!resonance::world_workspace_viewport_contains(
                                static_cast<int>(mouse_x),
                                static_cast<int>(mouse_y),
                                window_width,
                                window_height)) {
                            continue;
                        }
                        const float wheel_delta = static_cast<float>(event.wheel.y);
                        if ((SDL_GetModState() & (SDL_KMOD_CTRL | SDL_KMOD_GUI)) != 0U) {
                            scene.adjust_editor_viewport_zoom(wheel_delta * 0.25F);
                        } else {
                            scene.adjust_editor_selection_primary(wheel_delta);
                        }
                    }
                }
            }

            scene.update();
            SDL_SetWindowTitle(window, scene.debug_summary().c_str());
            int window_width = 0;
            int window_height = 0;
            SDL_GetWindowSize(window, &window_width, &window_height);
            draw_scene(renderer, scene, window_width, window_height);
            app.advance_frame(0.016);
            SDL_Delay(16);

            ++frames;
            if (auto_close && frames >= 4) {
                running = false;
            }
        }

        final_summary = scene.debug_summary();
        std::cout << final_summary << '\n';
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return (!final_summary.empty() && app.is_headless_ready()) ? 0 : 1;
#endif
}
