#pragma once

#include "animation/greeters/seasonal/seasonal_greeter_animation.hpp"

class SummerGreeterAnimation final : public SeasonalGreeterAnimation {
public:
    using SeasonalGreeterAnimation::SeasonalGreeterAnimation;

private:
    void drawScene(NanoCanvas<128, 64, 1>& canvas) const override;
    double revealThreshold(int col, int row) const override;
};
