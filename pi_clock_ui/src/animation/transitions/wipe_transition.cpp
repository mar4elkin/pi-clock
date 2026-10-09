#include "animation/transitions/wipe_transition.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <utility>

WipeTransition::WipeTransition(
    double duration,
    Callback on_midpoint,
    Callback on_finished
)
    : duration_(duration),
      on_midpoint_(std::move(on_midpoint)),
      on_finished_(std::move(on_finished))
{
    if (!std::isfinite(duration_) || duration_ <= 0.0) {
        throw std::invalid_argument(
            "Animation duration must be positive and finite"
        );
    }
}

void WipeTransition::onEnter() {
    elapsed_ = 0.0;
    phase_ = Phase::Covering;
}

void WipeTransition::update(const RenderContext& context) {
    const double dt = std::max(0.0, context.dt);
    const double half = duration_ / 2.0;

    switch (phase_) {
        case Phase::Covering:
            elapsed_ = std::min(elapsed_ + dt, half);

            if (elapsed_ >= half) {
                phase_ = Phase::Covered;

                if (on_midpoint_) {
                    on_midpoint_();
                }
            }
            break;

        case Phase::Covered:
            phase_ = Phase::Revealing;
            elapsed_ = 0.0;
            break;

        case Phase::Revealing:
            elapsed_ = std::min(elapsed_ + dt, half);

            if (elapsed_ >= half) {
                phase_ = Phase::Finished;
            }
            break;

        case Phase::Finished:
            break;
    }
}

void WipeTransition::render(
    NanoCanvas<128, 64, 1>& canvas
) {
    if (phase_ == Phase::Finished) {
        return;
    }

    constexpr int width = 128;
    constexpr int height = 64;

    const double half = duration_ / 2.0;
    const double progress = elapsed_ / half;

    canvas.setColor(0xFFFF);

    switch (phase_) {
        case Phase::Covering: {
            const int covered_width = static_cast<int>(
                std::round(width * progress)
            );

            if (covered_width > 0) {
                canvas.fillRect(
                    0, 0,
                    covered_width - 1, height - 1
                );
            }
            break;
        }

        case Phase::Covered:
            canvas.fillRect(0, 0, width - 1, height - 1);
            break;

        case Phase::Revealing: {
            const int start_x = static_cast<int>(
                std::round(width * progress)
            );

            if (start_x < width) {
                canvas.fillRect(
                    start_x, 0,
                    width - 1, height - 1
                );
            }
            break;
        }

        case Phase::Finished:
            break;
    }
}

bool WipeTransition::isFinished() const {
    return phase_ == Phase::Finished;
}

void WipeTransition::onExit() {
    if (on_finished_) {
        on_finished_();
    }
}