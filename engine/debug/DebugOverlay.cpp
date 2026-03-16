#include "engine/debug/DebugOverlay.h"

#include <algorithm>
#include <iomanip>
#include <sstream>
#include <string>

namespace resonance {
namespace {

constexpr float kCharacterWidth = 8.0F;
constexpr float kLineHeight = 10.0F;
constexpr float kHorizontalPadding = 8.0F;
constexpr float kVerticalPadding = 8.0F;

std::string format_audio_metric(const std::string& label, float value) {
    std::ostringstream stream;
    stream << std::fixed << std::setprecision(2) << value;
    return label + ": " + stream.str();
}

}  // namespace

std::vector<std::string> DebugOverlay::build_lines(
    const std::string& region_id,
    const std::string& music_state,
    const TraceLog& trace_log
) const {
    return build_lines(
        region_id,
        music_state,
        std::string{},
        std::string{},
        std::string{},
        std::string{},
        std::string{},
        trace_log,
        {},
        {});
}

std::vector<std::string> DebugOverlay::build_lines(
    const std::string& region_id,
    const std::string& music_state,
    const std::string& event_id,
    const std::string& action_prompt,
    const std::string& story_text,
    const std::string& nearby_anchor_id,
    const std::string& active_anchor_id,
    const TraceLog& trace_log,
    OverlayAudioTelemetry audio_telemetry,
    OverlayNarrativeTelemetry narrative_telemetry
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
    if (!nearby_anchor_id.empty()) {
        lines.push_back("Nearby Anchor: " + nearby_anchor_id);
    }
    if (!active_anchor_id.empty()) {
        lines.push_back("Active Anchor: " + active_anchor_id);
    }

    lines.push_back(format_audio_metric("Story Focus", audio_telemetry.story_focus));
    lines.push_back(format_audio_metric("Event Emphasis", audio_telemetry.event_emphasis));
    lines.push_back(format_audio_metric("Event Duck", audio_telemetry.event_duck));
    lines.push_back(format_audio_metric("Ambient Boost", audio_telemetry.ambient_boost));
    lines.push_back(format_audio_metric("Bgm Gain", audio_telemetry.bgm_gain));
    lines.push_back(format_audio_metric("Ambient Gain", audio_telemetry.ambient_gain_multiplier));
    lines.push_back("Memories: " + std::to_string(narrative_telemetry.memories_discovered));
    lines.push_back(
        "Latest Memory: " +
        (narrative_telemetry.latest_memory_id.empty() ? std::string{"<none>"} : narrative_telemetry.latest_memory_id));
    if (narrative_telemetry.memory_chain_total > 0U) {
        lines.push_back(
            "Memory Chain: " + std::to_string(narrative_telemetry.memory_chain_stage) + "/" +
            std::to_string(narrative_telemetry.memory_chain_total));
        lines.push_back(
            "Station Chain: " +
            (narrative_telemetry.station_chain_event.empty()
                    ? std::string{"<none>"}
                    : narrative_telemetry.station_chain_event));
        lines.push_back(
            "Next Memory: " +
            (narrative_telemetry.next_memory_id.empty()
                    ? std::string{"<complete>"}
                    : narrative_telemetry.next_memory_id));
        lines.push_back(
            "Next Station Upgrade: " +
            (narrative_telemetry.next_station_upgrade.empty()
                    ? std::string{"<complete>"}
                    : narrative_telemetry.next_station_upgrade));
    }
    lines.push_back(
        "Visible Events: " +
        (narrative_telemetry.visible_events.empty() ? std::string{"<none>"} : narrative_telemetry.visible_events));
    lines.push_back(
        "Next Event Unlock: " +
        (narrative_telemetry.next_event_unlock.empty() ? std::string{"<none>"} : narrative_telemetry.next_event_unlock));

    for (const auto& entry : trace_log.entries()) {
        lines.push_back("Trace: " + entry);
    }

    return lines;
}

std::vector<std::string> DebugOverlay::build_journal_lines(
    const std::vector<MemoryJournalEntry>& entries,
    OverlayNarrativeTelemetry narrative_telemetry
) const {
    std::vector<std::string> lines;
    lines.push_back("Journal");
    lines.push_back("Entries: " + std::to_string(entries.size()));
    if (narrative_telemetry.memory_chain_total > 0U) {
        lines.push_back(
            "Memory Chain: " + std::to_string(narrative_telemetry.memory_chain_stage) + "/" +
            std::to_string(narrative_telemetry.memory_chain_total));
        lines.push_back(
            "Station Chain: " +
            (narrative_telemetry.station_chain_event.empty()
                    ? std::string{"<none>"}
                    : narrative_telemetry.station_chain_event));
        lines.push_back(
            "Next Memory: " +
            (narrative_telemetry.next_memory_id.empty()
                    ? std::string{"<complete>"}
                    : narrative_telemetry.next_memory_id));
        lines.push_back(
            "Next Station Upgrade: " +
            (narrative_telemetry.next_station_upgrade.empty()
                    ? std::string{"<complete>"}
                    : narrative_telemetry.next_station_upgrade));
    }
    lines.push_back(
        "Visible Events: " +
        (narrative_telemetry.visible_events.empty() ? std::string{"<none>"} : narrative_telemetry.visible_events));
    lines.push_back(
        "Next Event Unlock: " +
        (narrative_telemetry.next_event_unlock.empty() ? std::string{"<none>"} : narrative_telemetry.next_event_unlock));

    for (const auto& entry : entries) {
        lines.push_back("Entry: " + entry.id);
        lines.push_back("Region: " + entry.region_id);
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
