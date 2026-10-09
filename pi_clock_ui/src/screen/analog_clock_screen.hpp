#pragma once

#include "lcdgfx.h"
#include "screen/screen.hpp"

struct Point {
    int x;
    int y;
};

class AnalogClockScreen : public BaseScreen {
public:
    void update(const RenderContext& context) override;
    void render(NanoCanvas<128, 64, 1>& canvas) override;

private:
    Point calculatePosition(
        int length,
        double value
    ) const;

    std::tm local_time_{};
};