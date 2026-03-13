#include "engine/debug/DebugOverlay.h"

#include <vector>

int main() {
    resonance::DebugOverlay overlay;

    const auto metrics = overlay.measure_lines({"Region: meadow", "Music: mysterious", "Trace: Event: distant_bell"});
    const auto empty_metrics = overlay.measure_lines({});

    return (metrics.width > empty_metrics.width &&
            metrics.height > empty_metrics.height &&
            empty_metrics.width > 0.0F &&
            empty_metrics.height > 0.0F)
        ? 0
        : 1;
}
