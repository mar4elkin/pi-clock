#include "animation/greeters/seasonal/greeter_drawing.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstring>

namespace greeter_detail {

static std::uint32_t hash(std::uint32_t x) {
    x ^= x >> 16;
    x *= 0x7feb352dU;
    x ^= x >> 15;
    x *= 0x846ca68bU;
    return x ^ (x >> 16);
}

double noise(int x, int y, int salt) {
    const std::uint32_t seed = static_cast<std::uint32_t>(
        x * 8179 + y * 131071 + salt * 524287 + 19);
    return static_cast<double>(hash(seed) & 0xffffU) / 65535.0;
}

double clamp01(double t) { return std::clamp(t, 0.0, 1.0); }
double easeOut(double t) {
    t = clamp01(t);
    return 1.0 - (1.0 - t) * (1.0 - t) * (1.0 - t);
}

double smooth(double t) {
    t = clamp01(t);
    return t * t * (3.0 - 2.0 * t);
}

int iround(double v) { return static_cast<int>(std::lround(v)); }

Point polar(int cx, int cy, double radians, double radius) {
    return {cx + iround(std::cos(radians) * radius),
            cy + iround(std::sin(radians) * radius)};
}

void dot(Canvas& c, int x, int y) {
    if (x >= 0 && x < 128 && y >= 0 && y < 64) c.putPixel(x, y);
}

// All endpoints are clamped: the driver never sees negative OLED coordinates.
void line(Canvas& c, int x1, int y1, int x2, int y2) {
    c.drawLine(std::clamp(x1, 0, 127), std::clamp(y1, 0, 63),
               std::clamp(x2, 0, 127), std::clamp(y2, 0, 63));
}

void centered(Canvas& c, const char* str, int y) {
    c.setFixedFont(ssd1306xled_font6x8);
    const int x = (128 - static_cast<int>(std::strlen(str)) * 6) / 2;
    c.printFixed(std::max(0, x), y, str, STYLE_NORMAL);
}

void sparkle(Canvas& c, int cx, int cy, int r) {
    dot(c, cx, cy);
    if (r > 0) {
        line(c, cx - r, cy, cx + r, cy);
        line(c, cx, cy - r, cx, cy + r);
    }
}

void fullWhite(Canvas& c, const char* heading, const char* subheading) {
    c.setColor(0xFFFF);
    c.fillRect(0, 0, 127, 63);
    c.setColor(0x0000);
    c.fillRect(6, 17, 121, 46);
    c.setColor(0xFFFF);
    centered(c, heading, 22);
    centered(c, subheading, 34);
}


} // namespace greeter_detail
