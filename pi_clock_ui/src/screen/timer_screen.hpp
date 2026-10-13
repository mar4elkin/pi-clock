#pragma once

#include "screen/screen.hpp"

class TimerScreen : public BaseScreen {
public:
    void update(const RenderContext& context) override;
    void render(NanoCanvas<128, 64, 1>& canvas) override;
};