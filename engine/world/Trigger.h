#pragma once

namespace resonance {

struct WorldPosition {
    float x = 0.0F;
    float y = 0.0F;
};

class Trigger {
public:
    Trigger(float x, float y, float width, float height);

    bool contains(WorldPosition position) const;

private:
    float x_;
    float y_;
    float width_;
    float height_;
};

}  // namespace resonance
