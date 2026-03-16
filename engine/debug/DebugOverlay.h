#pragma once

#include "engine/debug/TraceLog.h"
#include "engine/debug/MemoryJournalEntry.h"

#include <string>
#include <vector>

namespace resonance {

struct OverlayMetrics {
    float width = 0.0F;
    float height = 0.0F;
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
        const TraceLog& trace_log
    ) const;

    std::vector<std::string> build_journal_lines(
        const std::vector<MemoryJournalEntry>& entries
    ) const;

    OverlayMetrics measure_lines(const std::vector<std::string>& lines) const;
};

}  // namespace resonance
