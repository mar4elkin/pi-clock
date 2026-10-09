#pragma once

#include <functional>
#include "animation/animation.hpp"

// Shared boot-greeter lifecycle for ALL four seasons.
// Derived classes provide the artwork, reveal geometry and optional motion.
class SeasonalGreeterAnimation : public BaseAnimation {
public:
    using Callback = std::function<void()>;

    explicit SeasonalGreeterAnimation(
        Callback on_finished = {},
        Callback on_reveal = {},
        double reveal_start = 2.55,
        double end_time = 3.85
    );

    void onEnter() final;
    void update(const RenderContext& context) final;
    void render(NanoCanvas<128, 64, 1>& canvas) final;
    void onExit() final;
    bool isFinished() const final;

protected:
    double elapsed_ = 0.0;

    // Optional hooks for effects requiring state, e.g. winter snow particles.
    virtual void resetScene() {}
    virtual void advanceScene(double dt) { (void)dt; }

    // The visual behavior belongs to each concrete season.
    virtual void drawScene(NanoCanvas<128, 64, 1>& canvas) const = 0;
    virtual double revealThreshold(int col, int row) const = 0;

private:
    void drawRevealMask(NanoCanvas<128, 64, 1>& canvas,
                        double progress) const;

    const double reveal_start_;
    const double end_time_;
    Callback on_finished_;
    Callback on_reveal_;
    bool reveal_notified_ = false;
};
