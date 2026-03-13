#include "engine/core/Application.h"

int main() {
    resonance::Application app;
    return app.is_headless_ready() ? 0 : 1;
}
