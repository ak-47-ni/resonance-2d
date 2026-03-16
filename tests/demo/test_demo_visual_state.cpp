#include "engine/event/EventData.h"
#include "game/demo/DebugRoute.h"
#include "game/demo/DemoScene.h"

#include <string>

int main() {
    const auto bundle = resonance::load_demo_content("assets/data");
    resonance::DemoScene scene(bundle);

    scene.set_player_position({96.0F, 80.0F});
    scene.update();

    const auto player = scene.player_position();
    const auto visuals = scene.story_anchor_visuals();
    const auto summary = scene.debug_summary();

    scene.interact();
    scene.update();
    const auto story_summary = scene.debug_summary();

    scene.set_player_position({999.0F, 999.0F});
    scene.update();
    const auto cleared = scene.debug_summary();
    const auto route_summaries = resonance::run_memory_chain_debug_route(bundle);

    const bool has_region = summary.find("Region=meadow") != std::string::npos;
    const bool has_music = summary.find("Music=mysterious") != std::string::npos;
    const bool has_event = summary.find("Event=distant_bell") != std::string::npos;
    const bool has_story_focus = summary.find("StoryFocus=1.00") != std::string::npos;
    const bool has_event_emphasis = summary.find("EventEmphasis=1.00") != std::string::npos;
    const bool has_event_duck = summary.find("EventDuck=0.85") != std::string::npos;
    const bool has_ambient_boost = summary.find("AmbientBoost=0.65") != std::string::npos;
    const bool has_bgm_gain = summary.find("BgmGain=0.70") != std::string::npos;
    const bool has_ambient_gain = summary.find("AmbientGain=1.86") != std::string::npos;
    const bool has_zero_memories = summary.find("Memories=0") != std::string::npos;
    const bool has_no_latest_memory = summary.find("LatestMemory=<none>") != std::string::npos;
    const bool has_chain_stage_zero = summary.find("ChainStage=0/3") != std::string::npos;
    const bool has_station_chain_base = summary.find("StationChain=echoing_announcement") != std::string::npos;
    const bool has_next_memory_zero = summary.find("NextMemory=meadow-swing") != std::string::npos;
    const bool has_next_station_upgrade_zero = summary.find("NextStationUpgrade=platform_convergence") != std::string::npos;
    const bool has_nearby_anchor = summary.find("NearbyAnchor=meadow-swing") != std::string::npos;
    const bool has_story_anchor = story_summary.find("StoryAnchor=meadow-swing") != std::string::npos;
    const bool still_has_nearby_anchor = story_summary.find("NearbyAnchor=meadow-swing") != std::string::npos;
    const bool has_one_memory = story_summary.find("Memories=1") != std::string::npos;
    const bool has_latest_memory = story_summary.find("LatestMemory=meadow-swing") != std::string::npos;
    const bool has_chain_stage_one = story_summary.find("ChainStage=1/3") != std::string::npos;
    const bool has_station_chain_one = story_summary.find("StationChain=echoing_announcement") != std::string::npos;
    const bool has_next_memory_one = story_summary.find("NextMemory=ruins-gate") != std::string::npos;
    const bool has_next_station_upgrade_one = story_summary.find("NextStationUpgrade=platform_convergence") != std::string::npos;
    const bool cleared_event = cleared.find("Event=<none>") != std::string::npos;
    const bool cleared_story_focus = cleared.find("StoryFocus=0.00") != std::string::npos;
    const bool cleared_event_emphasis = cleared.find("EventEmphasis=0.00") != std::string::npos;
    const bool cleared_event_duck = cleared.find("EventDuck=1.00") != std::string::npos;
    const bool cleared_ambient_boost = cleared.find("AmbientBoost=1.00") != std::string::npos;
    const bool cleared_bgm_gain = cleared.find("BgmGain=1.00") != std::string::npos;
    const bool cleared_ambient_gain = cleared.find("AmbientGain=1.00") != std::string::npos;
    const bool cleared_memory_count = cleared.find("Memories=1") != std::string::npos;
    const bool cleared_latest_memory = cleared.find("LatestMemory=meadow-swing") != std::string::npos;
    const bool cleared_chain_stage = cleared.find("ChainStage=1/3") != std::string::npos;
    const bool cleared_station_chain = cleared.find("StationChain=echoing_announcement") != std::string::npos;
    const bool cleared_next_memory = cleared.find("NextMemory=ruins-gate") != std::string::npos;
    const bool cleared_next_station_upgrade = cleared.find("NextStationUpgrade=platform_convergence") != std::string::npos;
    const bool cleared_nearby_anchor = cleared.find("NearbyAnchor=") == std::string::npos;
    const bool has_route_start = route_summaries.size() >= 5U && route_summaries.front().find("RouteStep=meadow-start") != std::string::npos;
    const bool has_route_stage_two = route_summaries.size() >= 5U && route_summaries[3].find("Event=platform_convergence") != std::string::npos;
    const bool has_route_stage_three = route_summaries.size() >= 5U && route_summaries[4].find("Event=terminal_refrain") != std::string::npos;

    return (scene.region_count() == 4U &&
            visuals.size() == 4U &&
            player.x == 96.0F &&
            player.y == 80.0F &&
            has_region &&
            has_music &&
            has_event &&
            has_story_focus &&
            has_event_emphasis &&
            has_event_duck &&
            has_ambient_boost &&
            has_bgm_gain &&
            has_ambient_gain &&
            has_zero_memories &&
            has_no_latest_memory &&
            has_chain_stage_zero &&
            has_station_chain_base &&
            has_next_memory_zero &&
            has_next_station_upgrade_zero &&
            has_nearby_anchor &&
            has_story_anchor &&
            still_has_nearby_anchor &&
            has_one_memory &&
            has_latest_memory &&
            has_chain_stage_one &&
            has_station_chain_one &&
            has_next_memory_one &&
            has_next_station_upgrade_one &&
            cleared_event &&
            cleared_story_focus &&
            cleared_event_emphasis &&
            cleared_event_duck &&
            cleared_ambient_boost &&
            cleared_bgm_gain &&
            cleared_ambient_gain &&
            cleared_memory_count &&
            cleared_latest_memory &&
            cleared_chain_stage &&
            cleared_station_chain &&
            cleared_next_memory &&
            cleared_next_station_upgrade &&
            cleared_nearby_anchor &&
            has_route_start &&
            has_route_stage_two &&
            has_route_stage_three)
        ? 0
        : 1;
}
