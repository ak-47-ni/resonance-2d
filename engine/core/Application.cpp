#include "engine/core/Application.h"

namespace resonance {

Application::Application(ApplicationOptions options)
    : options_(options), ready_(true), running_(false) {}

void Application::advance_frame(double delta_seconds) {
    time_.tick(delta_seconds);
    running_ = true;
}

void Application::request_stop() {
    running_ = false;
}

bool Application::is_headless_ready() const {
    return ready_;
}

bool Application::is_running() const {
    return running_;
}

int Application::run() {
    advance_frame(0.0);
    request_stop();
    return is_headless_ready() ? 0 : 1;
}

const Time& Application::time() const {
    return time_;
}

}  // namespace resonance
