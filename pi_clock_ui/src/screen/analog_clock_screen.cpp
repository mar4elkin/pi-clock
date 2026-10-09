#include "analog_clock_screen.hpp"

#include <cstdio>
#include <numbers>
#include <cmath>


void AnalogClockScreen::update(const RenderContext& context) {
    local_time_ = context.local_time;
}

void AnalogClockScreen::render(NanoCanvas<128, 64, 1>& canvas)
{
    canvas.drawCircle(center_x, center_y, 29);

    Point second = calculatePosition(25, local_time_.tm_sec);

    Point minute = calculatePosition(
        20, local_time_.tm_min + local_time_.tm_sec / 60.0
    );

    Point hour = calculatePosition(
        15,
        (local_time_.tm_hour % 12) * 5.0
        + local_time_.tm_min / 12.0
    );

    canvas.drawLine(center_x, center_y, hour.x, hour.y);
    canvas.drawLine(center_x, center_y, minute.x, minute.y);
    canvas.drawLine(center_x, center_y, second.x, second.y);
}

Point AnalogClockScreen::calculatePosition(
    int length, double value) const
{
    const double angle =
        (value / 60.0) * 2.0 * std::numbers::pi;

    return {
        center_x + static_cast<int>(
            std::round(length * std::sin(angle))
        ),
        center_y - static_cast<int>(
            std::round(length * std::cos(angle))
        )
    };
}