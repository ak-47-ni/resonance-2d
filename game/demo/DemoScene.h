#pragma once

#include "engine/audio/AudioDirector.h"
#include "engine/debug/DebugOverlay.h"
#include "engine/debug/MemoryJournalEntry.h"
#include "engine/debug/TraceLog.h"
#include "engine/event/EventData.h"
#include "engine/event/EventDirector.h"
#include "engine/world/World.h"

#include <filesystem>
#include <string>
#include <vector>

namespace resonance {

struct StoryAnchorVisual {
    std::string id;
    WorldPosition position;
    float activation_radius = 0.0F;
    bool is_nearby = false;
    bool is_active = false;
    bool is_selected = false;
};

class DemoScene {
public:
    explicit DemoScene(DemoContentBundle bundle);
    explicit DemoScene(std::vector<RegionData> regions);

    void set_player_position(WorldPosition position);
    void move_player(WorldPosition delta);
    void update();
    void interact();
    bool select_region_at(WorldPosition position);
    bool select_story_anchor_at(WorldPosition position);
    bool nudge_editor_selection(WorldPosition delta);
    bool adjust_editor_selection_primary(float delta);
    bool clear_editor_selection();
    bool move_selected_region(WorldPosition delta);
    bool resize_selected_region(float width_delta, float height_delta);
    bool move_selected_story_anchor(WorldPosition delta);
    bool adjust_selected_story_anchor_radius(float delta);
    bool save_editor_document(const std::filesystem::path& data_root);
    void toggle_journal();
    void toggle_editor_mode();

    std::string current_region_id() const;
    std::string current_music_state() const;
    std::string current_event_id() const;
    float current_story_focus() const;
    float current_event_emphasis() const;
    float current_bgm_gain() const;
    float current_ambient_gain_multiplier() const;
    EventMixProfile current_event_mix_profile() const;
    std::string current_story_anchor_id() const;
    std::string current_interaction_prompt() const;
    std::string active_story_text() const;
    std::string selected_region_id() const;
    bool journal_is_open() const;
    bool editor_mode_active() const;
    std::vector<StoryAnchorVisual> story_anchor_visuals() const;
    std::vector<MemoryJournalEntry> memory_journal_entries() const;
    std::vector<std::string> overlay_lines() const;
    std::string debug_summary() const;
    WorldPosition player_position() const;
    std::size_t region_count() const;
    const std::vector<RegionData>& regions() const;

private:
    EventContext debug_event_context() const;
    const StoryAnchorData* find_nearby_story_anchor(const std::string& region_id) const;
    const StoryAnchorData* find_story_anchor_by_id(const std::string& anchor_id) const;
    std::size_t memory_chain_stage() const;
    std::string latest_memory_id() const;
    std::string next_event_unlock_text() const;
    std::string next_memory_id() const;
    std::string next_station_upgrade() const;
    std::string station_chain_event() const;
    std::string visible_events_text() const;
    float story_anchor_focus(const StoryAnchorData& anchor) const;

    World world_;
    AudioDirector audio_director_;
    EventDirector event_director_;
    DebugOverlay debug_overlay_;
    TraceLog trace_log_{8U};
    std::vector<StoryAnchorData> story_anchors_;
    std::vector<MemoryJournalEntry> memory_journal_entries_;
    std::string last_region_id_;
    std::string last_region_event_id_;
    std::string current_event_id_;
    std::string current_event_music_state_;
    EventMixProfile current_event_mix_profile_;
    std::string nearby_story_anchor_id_;
    std::string interaction_prompt_;
    std::string active_story_anchor_id_;
    std::string selected_region_id_;
    std::string selected_story_anchor_id_;
    std::string active_story_text_;
    float current_story_focus_ = 0.0F;
    float current_event_emphasis_ = 0.0F;
    bool journal_is_open_ = false;
    bool editor_mode_ = false;
    bool editor_dirty_ = false;
    std::string editor_save_status_;
    double seconds_since_last_major_event_ = 120.0;
    std::vector<std::string> overlay_lines_;
};

}  // namespace resonance
