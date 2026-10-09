#pragma once

#include <functional>

#include "animation/animation.hpp"

class WipeTransition : public BaseAnimation {
public:
    using Callback = std::function<void()>;

    WipeTransition(
        double duration,
        Callback on_midpoint,
        Callback on_finished
    );

    void onEnter() override;
    void update(const RenderContext& context) override;
    void render(NanoCanvas<128, 64, 1>& canvas) override;
    void onExit() override;

    bool isFinished() const override;

private:
    enum class Phase {
        Covering,
        Covered,
        Revealing,
        Finished
    };

    double duration_;
    double elapsed_ = 0.0;

    Phase phase_ = Phase::Covering;

    Callback on_midpoint_;
    Callback on_finished_;
};