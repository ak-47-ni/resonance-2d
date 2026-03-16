#include "game/demo/DemoScene.h"

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

void DemoScene::toggle_journal() {
    journal_is_open_ = !journal_is_open_;
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

bool DemoScene::journal_is_open() const {
    return journal_is_open_;
}

std::vector<StoryAnchorVisual> DemoScene::story_anchor_visuals() const {
    std::vector<StoryAnchorVisual> visuals;
    visuals.reserve(story_anchors_.size());

    for (const auto& anchor : story_anchors_) {
        visuals.push_back(StoryAnchorVisual{
            anchor.id,
            anchor.position,
            anchor.id == nearby_story_anchor_id_,
            anchor.id == active_story_anchor_id_,
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
