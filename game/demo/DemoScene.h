#pragma once

#include "engine/audio/AudioDirector.h"
#include "engine/debug/DebugOverlay.h"
#include "engine/debug/MemoryJournalEntry.h"
#include "engine/debug/TraceLog.h"
#include "engine/event/EventData.h"
#include "engine/event/EventDirector.h"
#include "engine/world/World.h"

#include <string>
#include <vector>

namespace resonance {

struct StoryAnchorVisual {
    std::string id;
    WorldPosition position;
    bool is_nearby = false;
    bool is_active = false;
};

class DemoScene {
public:
    explicit DemoScene(DemoContentBundle bundle);
    explicit DemoScene(std::vector<RegionData> regions);

    void set_player_position(WorldPosition position);
    void move_player(WorldPosition delta);
    void update();
    void interact();
    void toggle_journal();

    std::string current_region_id() const;
    std::string current_music_state() const;
    std::string current_event_id() const;
    std::string current_story_anchor_id() const;
    std::string current_interaction_prompt() const;
    std::string active_story_text() const;
    bool journal_is_open() const;
    std::vector<StoryAnchorVisual> story_anchor_visuals() const;
    std::vector<MemoryJournalEntry> memory_journal_entries() const;
    std::vector<std::string> overlay_lines() const;
    std::string debug_summary() const;
    WorldPosition player_position() const;
    std::size_t region_count() const;
    const std::vector<RegionData>& regions() const;

private:
    const StoryAnchorData* find_nearby_story_anchor(const std::string& region_id) const;

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
    std::string nearby_story_anchor_id_;
    std::string interaction_prompt_;
    std::string active_story_anchor_id_;
    std::string active_story_text_;
    bool journal_is_open_ = false;
    double seconds_since_last_major_event_ = 120.0;
    std::vector<std::string> overlay_lines_;
};

}  // namespace resonance
