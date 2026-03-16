#include "game/demo/DemoScene.h"

#include <algorithm>
#include <sstream>
#include <unordered_set>
#include <utility>

namespace resonance {

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
    const auto region_id = world_.current_region_id();
    const bool region_changed = region_id != last_region_id_;

    if (region_changed) {
        current_event_id_.clear();
        current_event_music_state_.clear();
        last_region_event_id_.clear();
        seconds_since_last_major_event_ = 120.0;
        last_region_id_ = region_id;
    }

    nearby_story_anchor_id_.clear();
    interaction_prompt_.clear();
    audio_director_.clear_story_cue();
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
            }
            current_event_id_ = event->id;
            current_event_music_state_ = event->requested_music_state;
            last_region_event_id_ = event->id;
            seconds_since_last_major_event_ = 0.0;
        }

        if (!current_event_music_state_.empty()) {
            audio_director_.request_story_cue(current_event_music_state_);
        }

        if (const auto* anchor = find_nearby_story_anchor(region_id); anchor != nullptr) {
            nearby_story_anchor_id_ = anchor->id;
            interaction_prompt_ = anchor->prompt_text;
        }
    } else {
        current_event_id_.clear();
        current_event_music_state_.clear();
        last_region_event_id_.clear();
    }

    if (journal_is_open_) {
        overlay_lines_ = debug_overlay_.build_journal_lines(memory_journal_entries_);
    } else {
        overlay_lines_ = debug_overlay_.build_lines(
            region_id,
            audio_director_.current_music_state(),
            current_event_id_,
            interaction_prompt_,
            active_story_text_,
            trace_log_);
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
           << " | Event=" << (current_event_id().empty() ? "<none>" : current_event_id());
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
