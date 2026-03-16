#include "engine/debug/DebugOverlay.h"

#include <algorithm>
#include <string>

namespace resonance {
namespace {

constexpr float kCharacterWidth = 8.0F;
constexpr float kLineHeight = 10.0F;
constexpr float kHorizontalPadding = 8.0F;
constexpr float kVerticalPadding = 8.0F;

}  // namespace

std::vector<std::string> DebugOverlay::build_lines(
    const std::string& region_id,
    const std::string& music_state,
    const TraceLog& trace_log
) const {
    return build_lines(region_id, music_state, {}, {}, {}, trace_log);
}

std::vector<std::string> DebugOverlay::build_lines(
    const std::string& region_id,
    const std::string& music_state,
    const std::string& event_id,
    const std::string& action_prompt,
    const std::string& story_text,
    const TraceLog& trace_log
) const {
    std::vector<std::string> lines;
    lines.push_back("Region: " + region_id);
    lines.push_back("Music: " + music_state);

    if (!event_id.empty()) {
        lines.push_back("Event: " + event_id);
    }
    if (!action_prompt.empty()) {
        lines.push_back("Action: " + action_prompt);
    }
    if (!story_text.empty()) {
        lines.push_back("Story: " + story_text);
    }

    for (const auto& entry : trace_log.entries()) {
        lines.push_back("Trace: " + entry);
    }

    return lines;
}

std::vector<std::string> DebugOverlay::build_journal_lines(
    const std::vector<MemoryJournalEntry>& entries
) const {
    std::vector<std::string> lines;
    lines.push_back("Journal");
    lines.push_back("Entries: " + std::to_string(entries.size()));

    for (const auto& entry : entries) {
        lines.push_back("Entry: " + entry.id);
        lines.push_back("Memory: " + entry.story_text);
    }

    return lines;
}

OverlayMetrics DebugOverlay::measure_lines(const std::vector<std::string>& lines) const {
    std::size_t max_width = 0U;
    for (const auto& line : lines) {
        max_width = std::max(max_width, line.size());
    }

    const std::size_t visible_lines = std::max<std::size_t>(1U, lines.size());
    return {
        static_cast<float>(max_width) * kCharacterWidth + (kHorizontalPadding * 2.0F),
        static_cast<float>(visible_lines) * kLineHeight + (kVerticalPadding * 2.0F),
    };
}

}  // namespace resonance
