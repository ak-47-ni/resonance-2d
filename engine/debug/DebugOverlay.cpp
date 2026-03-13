#include "engine/debug/DebugOverlay.h"

#include <algorithm>

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
    std::vector<std::string> lines;
    lines.push_back("Region: " + region_id);
    lines.push_back("Music: " + music_state);

    for (const auto& entry : trace_log.entries()) {
        lines.push_back("Trace: " + entry);
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
