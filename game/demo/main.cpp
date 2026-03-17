#include "engine/core/Application.h"
#include "engine/debug/DebugOverlay.h"
#include "engine/event/EventData.h"
#include "game/demo/DebugRoute.h"
#include "game/demo/DemoScene.h"

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

void draw_story_anchor_markers(SDL_Renderer* renderer, const resonance::DemoScene& scene) {
    for (const auto& visual : scene.story_anchor_visuals()) {
        SDL_FRect outer{visual.position.x - 4.0F, visual.position.y - 4.0F, 8.0F, 8.0F};
        if (visual.is_nearby) {
            SDL_SetRenderDrawColor(renderer, 255, 208, 120, 255);
        } else {
            SDL_SetRenderDrawColor(renderer, 80, 200, 220, 220);
        }
        SDL_RenderFillRect(renderer, &outer);

        if (visual.is_selected) {
            SDL_FRect selection_radius{
                visual.position.x - visual.activation_radius,
                visual.position.y - visual.activation_radius,
                visual.activation_radius * 2.0F,
                visual.activation_radius * 2.0F,
            };
            SDL_SetRenderDrawColor(renderer, 255, 120, 220, 200);
            SDL_RenderRect(renderer, &selection_radius);

            SDL_FRect selection{visual.position.x - 8.0F, visual.position.y - 8.0F, 16.0F, 16.0F};
            SDL_SetRenderDrawColor(renderer, 255, 120, 220, 255);
            SDL_RenderRect(renderer, &selection);
        }

        if (visual.is_active) {
            SDL_FRect inner{visual.position.x - 2.0F, visual.position.y - 2.0F, 4.0F, 4.0F};
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderFillRect(renderer, &inner);
        }
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

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    float y = 16.0F;
    for (const auto& line : lines) {
        SDL_RenderDebugText(renderer, 16.0F, y, line.c_str());
        y += 10.0F;
    }
}

void draw_scene(SDL_Renderer* renderer, const resonance::DemoScene& scene) {
    SDL_SetRenderDrawColor(renderer, 18, 18, 24, 255);
    SDL_RenderClear(renderer);

    for (const auto& region : scene.regions()) {
        SDL_FRect rect = to_rect(region.bounds);
        if (region.id == "meadow") {
            SDL_SetRenderDrawColor(renderer, 70, 120, 80, 255);
        } else if (region.id == "ruins") {
            SDL_SetRenderDrawColor(renderer, 110, 90, 90, 255);
        } else if (region.id == "lakeside") {
            SDL_SetRenderDrawColor(renderer, 60, 90, 150, 255);
        } else {
            SDL_SetRenderDrawColor(renderer, 110, 80, 130, 255);
        }
        SDL_RenderFillRect(renderer, &rect);

        if (region.id == scene.current_region_id()) {
            SDL_SetRenderDrawColor(renderer, 255, 240, 160, 255);
            SDL_RenderRect(renderer, &rect);
        }
        if (scene.selected_region_id() == region.id) {
            SDL_SetRenderDrawColor(renderer, 255, 120, 220, 255);
            SDL_RenderRect(renderer, &rect);
        }
    }

    draw_story_anchor_markers(renderer, scene);

    const auto player = scene.player_position();
    SDL_FRect player_rect{player.x - 6.0F, player.y - 6.0F, 12.0F, 12.0F};
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &player_rect);

    draw_overlay(renderer, scene);
    SDL_RenderPresent(renderer);
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

    SDL_Window* window = SDL_CreateWindow("resonance-2d", 640, 640, window_flags);
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
        int frames = 0;
        while (running) {
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_EVENT_QUIT) {
                    running = false;
                } else if (event.type == SDL_EVENT_KEY_DOWN) {
                    switch (event.key.key) {
                        case SDLK_ESCAPE:
                            running = false;
                            break;
                        case SDLK_LEFT:
                        case SDLK_A:
                            if (!scene.editor_mode_active() || !scene.nudge_editor_selection({-4.0F, 0.0F})) {
                                scene.move_player({-12.0F, 0.0F});
                            }
                            break;
                        case SDLK_RIGHT:
                        case SDLK_D:
                            if (!scene.editor_mode_active() || !scene.nudge_editor_selection({4.0F, 0.0F})) {
                                scene.move_player({12.0F, 0.0F});
                            }
                            break;
                        case SDLK_UP:
                        case SDLK_W:
                            if (!scene.editor_mode_active() || !scene.nudge_editor_selection({0.0F, -4.0F})) {
                                scene.move_player({0.0F, -12.0F});
                            }
                            break;
                        case SDLK_DOWN:
                        case SDLK_S:
                            if (!scene.editor_mode_active() || !scene.nudge_editor_selection({0.0F, 4.0F})) {
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
                        default:
                            break;
                    }
                } else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                    if (scene.editor_mode_active() && event.button.button == SDL_BUTTON_RIGHT) {
                        scene.clear_editor_selection();
                    }
                    if (scene.editor_mode_active() && event.button.button == SDL_BUTTON_LEFT) {
                        const resonance::WorldPosition click_position{static_cast<float>(event.button.x), static_cast<float>(event.button.y)};
                        if (!scene.select_story_anchor_at(click_position)) {
                            scene.select_region_at(click_position);
                        }
                    }
                } else if (event.type == SDL_EVENT_MOUSE_MOTION) {
                    if (scene.editor_mode_active() && ((event.motion.state & SDL_BUTTON_LMASK) != 0U)) {
                        const resonance::WorldPosition drag_delta{static_cast<float>(event.motion.xrel), static_cast<float>(event.motion.yrel)};
                        scene.nudge_editor_selection(drag_delta);
                    }
                } else if (event.type == SDL_EVENT_MOUSE_WHEEL) {
                    if (scene.editor_mode_active()) {
                        const float wheel_delta = static_cast<float>(event.wheel.y);
                        scene.adjust_editor_selection_primary(wheel_delta);
                    }
                }
            }

            scene.update();
            SDL_SetWindowTitle(window, scene.debug_summary().c_str());
            draw_scene(renderer, scene);
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
