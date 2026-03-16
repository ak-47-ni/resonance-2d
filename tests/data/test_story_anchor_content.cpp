#include "engine/event/EventData.h"

int main() {
    const auto bundle = resonance::load_demo_content("assets/data");
    return (!bundle.story_anchors.empty() && bundle.story_anchors.front().id == "meadow-swing") ? 0 : 1;
}
