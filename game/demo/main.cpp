#include "engine/core/Application.h"
#include "engine/debug/DebugOverlay.h"
#include "engine/event/EventData.h"
#include "game/demo/DemoScene.h"

#include <cstdlib>
#include <iostream>
#include <string>
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
    }

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
    resonance::DemoScene scene(bundle);
    scene.set_player_position({10.0F, 10.0F});
    scene.update();
    std::cout << scene.debug_summary() << '\n';
    return (!scene.overlay_lines().empty() && app.is_headless_ready()) ? 0 : 1;
#else
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << '\n';
        return 1;
    }

    if (!SDL_InitSubSystem(SDL_INIT_AUDIO)) {
        std::cerr << "SDL audio unavailable: " << SDL_GetError() << '\n';
    }

    const bool hidden = std::getenv("RESONANCE_DEMO_HIDDEN") != nullptr;
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
                        scene.move_player({-12.0F, 0.0F});
                        break;
                    case SDLK_RIGHT:
                    case SDLK_D:
                        scene.move_player({12.0F, 0.0F});
                        break;
                    case SDLK_UP:
                    case SDLK_W:
                        scene.move_player({0.0F, -12.0F});
                        break;
                    case SDLK_DOWN:
                    case SDLK_S:
                        scene.move_player({0.0F, 12.0F});
                        break;
                    default:
                        break;
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
