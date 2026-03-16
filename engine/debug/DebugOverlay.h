#pragma once

#include "engine/debug/MemoryJournalEntry.h"
#include "engine/debug/TraceLog.h"

#include <cstddef>
#include <string>
#include <vector>

namespace resonance {

struct OverlayMetrics {
    float width = 0.0F;
    float height = 0.0F;
};

struct OverlayAudioTelemetry {
    float story_focus = 0.0F;
    float event_emphasis = 0.0F;
    float event_duck = 0.0F;
    float ambient_boost = 0.0F;
    float bgm_gain = 1.0F;
    float ambient_gain_multiplier = 1.0F;
};

struct OverlayNarrativeTelemetry {
    std::size_t memories_discovered = 0U;
    std::string latest_memory_id;
    std::size_t memory_chain_stage = 0U;
    std::size_t memory_chain_total = 0U;
    std::string station_chain_event;
    std::string next_memory_id;
    std::string next_station_upgrade;
    std::string visible_events;
    std::string next_event_unlock;
};

class DebugOverlay {
public:
    std::vector<std::string> build_lines(
        const std::string& region_id,
        const std::string& music_state,
        const TraceLog& trace_log
    ) const;

    std::vector<std::string> build_lines(
        const std::string& region_id,
        const std::string& music_state,
        const std::string& event_id,
        const std::string& action_prompt,
        const std::string& story_text,
        const std::string& nearby_anchor_id,
        const std::string& active_anchor_id,
        const TraceLog& trace_log,
        OverlayAudioTelemetry audio_telemetry = {},
        OverlayNarrativeTelemetry narrative_telemetry = {}
    ) const;

    std::vector<std::string> build_journal_lines(
        const std::vector<MemoryJournalEntry>& entries,
        OverlayNarrativeTelemetry narrative_telemetry = {}
    ) const;

    OverlayMetrics measure_lines(const std::vector<std::string>& lines) const;
};

}  // namespace resonance
