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
        "meadow-swing",
        "meadow-swing",
        log,
        resonance::OverlayAudioTelemetry{1.0F, 1.0F, 0.85F, 0.65F, 0.70F, 1.86F},
        resonance::OverlayNarrativeTelemetry{
            1U,
            "meadow-swing",
            0U,
            3U,
            "echoing_announcement",
            "meadow-swing",
            "platform_convergence",
            "distant_bell,passing_shadow",
            "swing_memory_echo<-memory:meadow-swing",
        });
    const auto metrics = overlay.measure_lines(lines);
    const auto empty_metrics = overlay.measure_lines({});

    const bool has_nearby_anchor = std::find(lines.begin(), lines.end(), std::string{"Nearby Anchor: meadow-swing"}) != lines.end();
    const bool has_active_anchor = std::find(lines.begin(), lines.end(), std::string{"Active Anchor: meadow-swing"}) != lines.end();
    const bool has_bgm_gain = std::find(lines.begin(), lines.end(), std::string{"Bgm Gain: 0.70"}) != lines.end();
    const bool has_ambient_gain = std::find(lines.begin(), lines.end(), std::string{"Ambient Gain: 1.86"}) != lines.end();
    const bool has_visible_events = std::find(lines.begin(), lines.end(), std::string{"Visible Events: distant_bell,passing_shadow"}) != lines.end();
    const bool has_next_unlock = std::find(lines.begin(), lines.end(), std::string{"Next Event Unlock: swing_memory_echo<-memory:meadow-swing"}) != lines.end();

    return (has_nearby_anchor &&
            has_active_anchor &&
            has_bgm_gain &&
            has_ambient_gain &&
            has_visible_events &&
            has_next_unlock &&
            metrics.width > empty_metrics.width &&
            metrics.height > empty_metrics.height &&
            empty_metrics.width > 0.0F &&
            empty_metrics.height > 0.0F)
        ? 0
        : 1;
}
