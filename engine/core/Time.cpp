#include "engine/core/Time.h"

namespace resonance {

void Time::tick(double delta_seconds) {
    const double clamped_delta = delta_seconds < 0.0 ? 0.0 : delta_seconds;
    delta_seconds_ = clamped_delta;
    total_seconds_ += clamped_delta;
}

double Time::delta_seconds() const {
    return delta_seconds_;
}

double Time::total_seconds() const {
    return total_seconds_;
}

}  // namespace resonance
