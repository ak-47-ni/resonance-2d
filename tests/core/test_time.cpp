#include "engine/core/Time.h"

int main() {
    resonance::Time time;
    time.tick(0.016);
    time.tick(0.020);

    return (time.delta_seconds() > 0.0 && time.total_seconds() > 0.03) ? 0 : 1;
}
