#pragma once

#include "engine/core/Time.h"

namespace resonance {

struct ApplicationOptions {
    bool headless = false;
};

class Application {
public:
    explicit Application(ApplicationOptions options = {});

    void advance_frame(double delta_seconds);
    void request_stop();
    bool is_headless_ready() const;
    bool is_running() const;
    int run();

    const Time& time() const;

private:
    ApplicationOptions options_;
    Time time_;
    bool ready_;
    bool running_;
};

}  // namespace resonance
