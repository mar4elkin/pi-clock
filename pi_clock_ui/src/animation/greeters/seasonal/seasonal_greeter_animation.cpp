#include "animation/greeters/seasonal/seasonal_greeter_animation.hpp"
#include "animation/greeters/seasonal/greeter_drawing.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <utility>

using greeter_detail::Canvas;
using greeter_detail::clamp01;

SeasonalGreeterAnimation::SeasonalGreeterAnimation(
    Callback on_finished, Callback on_reveal,
    double reveal_start, double end_time)
    : reveal_start_(reveal_start),
      end_time_(end_time),
      on_finished_(std::move(on_finished)),
      on_reveal_(std::move(on_reveal)) {
    if (!std::isfinite(reveal_start_) || !std::isfinite(end_time_) ||
        reveal_start_ <= 0.0 || end_time_ <= reveal_start_) {
        throw std::invalid_argument("Invalid greeter timing");
    }
}

void SeasonalGreeterAnimation::onEnter() {
    elapsed_ = 0.0;
    reveal_notified_ = false;
    resetScene();
}

void SeasonalGreeterAnimation::update(const RenderContext& context) {
    if (isFinished()) return;

    const double dt = std::isfinite(context.dt)
        ? std::max(0.0, context.dt) : 0.0;
    const double before = elapsed_;
    elapsed_ = std::min(end_time_, elapsed_ + dt);

    if (!reveal_notified_ && before < reveal_start_ &&
        elapsed_ >= reveal_start_) {
        reveal_notified_ = true;
        if (on_reveal_) on_reveal_();
    }

    // Avoid unbounded particle movement after e.g. laptop suspend.
    advanceScene(elapsed_ - before);
}

void SeasonalGreeterAnimation::render(Canvas& canvas) {
    if (elapsed_ >= reveal_start_) {
        // Preserve the already-rendered clock underneath the reveal mask.
        const double progress = clamp01((elapsed_ - reveal_start_) /
                                        (end_time_ - reveal_start_));
        drawRevealMask(canvas, progress);
    } else {
        canvas.clear();
        canvas.setColor(0xFFFF);
        drawScene(canvas);
    }
}

void SeasonalGreeterAnimation::drawRevealMask(
    Canvas& canvas, double progress) const {
    canvas.setColor(0xFFFF);
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 16; ++col) {
            if (progress < revealThreshold(col, row)) {
                canvas.fillRect(col * 8, row * 8,
                                col * 8 + 7, row * 8 + 7);
            }
        }
    }
}

void SeasonalGreeterAnimation::onExit() {
    if (on_finished_) on_finished_();
}

bool SeasonalGreeterAnimation::isFinished() const {
    return elapsed_ >= end_time_;
}
