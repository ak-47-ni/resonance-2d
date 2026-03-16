#include "engine/debug/DebugOverlay.h"
#include "engine/debug/TraceLog.h"

#include <algorithm>
#include <string>
#include <vector>

int main() {
    resonance::DebugOverlay overlay;
    resonance::TraceLog log{4U};
    log.push("Event: distant_bell");

    const auto lines = overlay.build_lines(
        "meadow",
        "mysterious",
        "distant_bell",
        "Press E to listen",
        "A rusted swing clicks in the wind.",
        log);
    const auto metrics = overlay.measure_lines(lines);
    const auto empty_metrics = overlay.measure_lines({});

    const bool has_prompt = std::find(lines.begin(), lines.end(), std::string{"Action: Press E to listen"}) != lines.end();
    const bool has_story = std::find(lines.begin(), lines.end(), std::string{"Story: A rusted swing clicks in the wind."}) != lines.end();

    return (has_prompt &&
            has_story &&
            metrics.width > empty_metrics.width &&
            metrics.height > empty_metrics.height &&
            empty_metrics.width > 0.0F &&
            empty_metrics.height > 0.0F)
        ? 0
        : 1;
}
