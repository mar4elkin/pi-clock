#pragma once

#include "screen/screen.hpp"

enum class TimerScreenStatus {
    VIEWING,
    EDITING,
    RUNNING,
    PAUSED,
    DONE
};

enum class TimerScreenActions {
    SET_UP,
    SET_DOWN,
    RESET
};

class TimerScreen : public BaseScreen {
public:
    void update(const RenderContext& context) override;
    void render(NanoCanvas<128, 64, 1>& canvas) override;
    bool handleInput(InputAction action) override;
};