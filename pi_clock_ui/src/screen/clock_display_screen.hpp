#pragma once

#include "lcdgfx.h"
#include "screen/screen.hpp"

class ClockDisplayScreen : public BaseScreen {
public:
    void update(const RenderContext& context) override;
    void render(NanoCanvas<128, 64, 1>& canvas) override;

private:
    std::tm local_time_{};
};