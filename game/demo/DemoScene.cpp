#include "game/demo/DemoScene.h"

#include <sstream>
#include <utility>

namespace resonance {

DemoScene::DemoScene(DemoContentBundle bundle)
    : world_(bundle.regions), event_director_(std::move(bundle.events)) {
    audio_director_.register_music_states(bundle.music_states);
    for (const auto& region : bundle.regions) {
        audio_director_.set_region_music(region.id, region.default_music_state);
    }
}

DemoScene::DemoScene(std::vector<RegionData> regions)
    : DemoScene(DemoContentBundle{std::move(regions), {}, {}}) {}

void DemoScene::set_player_position(WorldPosition position) {
    world_.set_player_position(position);
}

void DemoScene::move_player(WorldPosition delta) {
    const WorldPosition current = world_.player_position();
    world_.set_player_position({current.x + delta.x, current.y + delta.y});
}

void DemoScene::update() {
    const auto region_id = world_.current_region_id();

    current_event_id_.clear();
    audio_director_.clear_story_cue();
    audio_director_.enter_region(region_id);

    if (!region_id.empty()) {
        const auto event = event_director_.pick_event({region_id, {}, 60.0});
        if (event.has_value()) {
            current_event_id_ = event->id;
            audio_director_.request_story_cue(event->requested_music_state);
            trace_log_.push("Event: " + current_event_id_);
        }
    }

    overlay_lines_ = debug_overlay_.build_lines(
        region_id,
        audio_director_.current_music_state(),
        trace_log_
    );
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

std::vector<std::string> DemoScene::overlay_lines() const {
    return overlay_lines_;
}

std::string DemoScene::debug_summary() const {
    std::ostringstream stream;
    stream << "Region=" << (current_region_id().empty() ? "<none>" : current_region_id())
           << " | Music=" << (current_music_state().empty() ? "<none>" : current_music_state())
           << " | Event=" << (current_event_id().empty() ? "<none>" : current_event_id());
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

}  // namespace resonance
