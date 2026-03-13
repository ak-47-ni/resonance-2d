#pragma once

namespace resonance {

class Time {
public:
    void tick(double delta_seconds);

    double delta_seconds() const;
    double total_seconds() const;

private:
    double delta_seconds_ = 0.0;
    double total_seconds_ = 0.0;
};

}  // namespace resonance
