#include "animation/greeters/seasonal/frost_greeter_animation.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <numbers>
#include <utility>

namespace {
constexpr double pi = std::numbers::pi;
constexpr int screen_width = 128;
constexpr int screen_height = 64;
constexpr int cx = 64;
constexpr int cy = 31;

struct Point { int x; int y; };

Point ray(int x, int y, double angle, double length) {
    return {
        x + static_cast<int>(std::lround(std::cos(angle) * length)),
        y + static_cast<int>(std::lround(std::sin(angle) * length))
    };
}

double smoothstep(double p) {
    p = std::clamp(p, 0.0, 1.0);
    return p * p * (3.0 - 2.0 * p);
}

// Stable pixel-by-pixel breakup of a crack: avoids a sudden cut to the reveal.
void fractureLine(NanoCanvas<128, 64, 1>& canvas,
                  Point from, Point to, double amount, double opacity,
                  std::uint32_t seed) {
    if (amount <= 0.0 || opacity <= 0.0) return;
    const Point end{
        from.x + static_cast<int>(std::lround((to.x - from.x) * amount)),
        from.y + static_cast<int>(std::lround((to.y - from.y) * amount))
    };
    int x = from.x, y = from.y;
    const int dx = std::abs(end.x - x), dy = std::abs(end.y - y);
    const int sx = x < end.x ? 1 : -1;
    const int sy = y < end.y ? 1 : -1;
    int err = dx - dy;
    for (;;) {
        std::uint32_t n = static_cast<std::uint32_t>(x * 137 + y * 73) ^ seed;
        n ^= n >> 16;
        n *= 0x7feb352dU;
        n ^= n >> 15;
        if (x >= 0 && x < screen_width && y >= 0 && y < screen_height &&
            static_cast<double>(n & 255U) / 255.0 < opacity) {
            canvas.putPixel(x, y);
        }
        if (x == end.x && y == end.y) break;
        const int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x += sx; }
        if (e2 < dx)  { err += dx; y += sy; }
    }
}
} // namespace

FrostGreeterAnimation::FrostGreeterAnimation(
    Callback on_finished, Callback on_reveal)
    : SeasonalGreeterAnimation(
          std::move(on_finished), std::move(on_reveal), 2.50, 3.80) {}

std::uint32_t FrostGreeterAnimation::hash(std::uint32_t x) {
    x ^= x >> 16;
    x *= 0x7feb352dU;
    x ^= x >> 15;
    x *= 0x846ca68bU;
    return x ^ (x >> 16);
}

double FrostGreeterAnimation::clamp01(double x) {
    return std::clamp(x, 0.0, 1.0);
}

double FrostGreeterAnimation::blockThreshold(int col, int row) {
    // Frost grows out from the crystal, with small irregularities.
    const double dx = (col + 0.5 - 8.0) / 8.0;
    const double dy = (row + 0.5 - 4.0) / 4.0;
    const double distance = std::min(1.0, std::hypot(dx, dy) / 1.42);
    const double jitter = static_cast<double>(
        hash(static_cast<std::uint32_t>(col * 773 + row * 9277)) & 255U
    ) / 255.0;
    return 0.76 * distance + 0.24 * jitter;
}

void FrostGreeterAnimation::resetScene() {
    for (std::size_t i = 0; i < particles_.size(); ++i) {
        auto& p = particles_[i];
        p.x = static_cast<float>(hash(static_cast<std::uint32_t>(i * 3 + 1)) % width_);
        p.y = static_cast<float>(hash(static_cast<std::uint32_t>(i * 3 + 2)) % height_);
        p.speed = static_cast<float>(12 + hash(static_cast<std::uint32_t>(i * 3 + 3)) % 36);
    }
}

void FrostGreeterAnimation::advanceScene(double dt) {
    // Snow drifts lazily at first; the spell pulls it into a sideways blizzard.
    const double storm = smoothstep((elapsed_ - 0.98) / 0.80);
    for (auto& p : particles_) {
        p.y -= static_cast<float>(p.speed * (1.0 + 0.7 * storm) * dt);
        p.x += static_cast<float>(p.speed * (0.12 + 1.45 * storm) * dt);

        while (p.y < 0.0f) p.y += height_;
        while (p.x >= width_) p.x -= width_;
    }
}

void FrostGreeterAnimation::drawSnow(NanoCanvas<128, 64, 1>& canvas) const {
    const double build = smoothstep(elapsed_ / 1.45);
    const double gust = smoothstep((elapsed_ - 0.98) / 0.65);
    const int count = 4 + static_cast<int>((particles_.size() - 4) * build);

    canvas.setColor(0xFFFF);
    for (int i = 0; i < count; ++i) {
        const auto& p = particles_[i];
        const int x = static_cast<int>(p.x);
        const int y = static_cast<int>(p.y);
        canvas.putPixel(x, y);

        if (i % 5 == 0 && build < 0.7) {
            // A few bright, static-looking twinkles in the quiet opening.
            if (x + 1 < screen_width) canvas.putPixel(x + 1, y);
            if (y + 1 < screen_height) canvas.putPixel(x, y + 1);
        }
        if (gust > 0.05 && i % 3 == 0) {
            const int trail = 1 + static_cast<int>(gust * (2 + i % 5));
            canvas.drawLine(x, y, std::max(0, x - trail),
                            std::min(screen_height - 1, y + 2));
        }
    }
}

void FrostGreeterAnimation::drawCrystal(NanoCanvas<128, 64, 1>& canvas) const {
    const double progress = smoothstep((elapsed_ - crystal_start_) / 0.86);
    if (progress <= 0.0) return;

    // Staged growth: trunk first, then the two rows of dendritic branches.
    const double length = 27.0 * progress;
    canvas.setColor(0xFFFF);

    for (int arm = 0; arm < 6; ++arm) {
        const double angle = arm * pi / 3.0 - pi / 2.0;
        const Point tip = ray(cx, cy, angle, length);
        canvas.drawLine(cx, cy, tip.x, tip.y);

        for (double fraction : {0.45, 0.72}) {
            if (progress < fraction) continue;
            const Point joint = ray(cx, cy, angle, 27.0 * fraction);
            const double branch_growth = smoothstep(
                (progress - fraction) / (1.0 - fraction));
            const double branch_length =
                (fraction < 0.5 ? 8.0 : 6.0) * branch_growth;

            for (int sign : {-1, 1}) {
                const Point branch = ray(joint.x, joint.y,
                                         angle + sign * 0.72, branch_length);
                canvas.drawLine(joint.x, joint.y, branch.x, branch.y);
            }
        }
    }

    // Tiny nucleus is visible before the six arms finish growing.
    canvas.drawCircle(cx, cy, progress > 0.6 ? 2 : 1);

    // A thin shockwave grows around the crystal just before the whiteout.
    const double wave = smoothstep((elapsed_ - 1.23) / 0.58);
    if (wave > 0.0) {
        const int radius = 4 + static_cast<int>(std::lround(wave * 26));
        // A sparse ring reads better than a solid circle on a 1-bit panel.
        for (int i = 0; i < 36; ++i) {
            if ((i + static_cast<int>(wave * 20)) % 3 == 0) continue;
            const Point p = ray(cx, cy, 2.0 * pi * i / 36.0, radius);
            canvas.putPixel(p.x, p.y);
        }
    }
}

void FrostGreeterAnimation::drawStorm(NanoCanvas<128, 64, 1>& canvas) const {
    const double intensity = smoothstep((elapsed_ - 1.04) / 0.70);
    if (intensity <= 0.0) return;

    const int streaks = static_cast<int>(28 * intensity);
    canvas.setColor(0xFFFF);

    for (int i = 0; i < streaks; ++i) {
        // Deterministic streaks: no frame-to-frame random noise/flicker.
        const double displacement = elapsed_ * (48.0 + 7.0 * (i % 5));
        const int x = static_cast<int>(std::fmod(
            hash(static_cast<std::uint32_t>(i * 31 + 19)) % 160 + displacement,
            160.0)) - 16;
        const int y = static_cast<int>(hash(static_cast<std::uint32_t>(i * 37 + 7)) % height_);
        if (x < 0 || x >= screen_width) continue;
        const int length = 2 + static_cast<int>(intensity * (4 + i % 6));
        canvas.drawLine(x, y, std::max(0, x - length),
                        std::min(screen_height - 1, y + std::min(3, length / 3)));
    }
}

void FrostGreeterAnimation::drawIceMask(
    NanoCanvas<128, 64, 1>& canvas, double progress) const {
    canvas.setColor(0xFFFF);

    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 16; ++col) {
            const double threshold = blockThreshold(col, row);
            const double amount = clamp01((progress - threshold + 0.09) / 0.09);
            if (amount <= 0.0) continue;

            const int x = col * 8;
            const int y = row * 8;
            if (amount >= 1.0) {
                canvas.fillRect(x, y, x + 7, y + 7);
            } else {
                // Granular leading edge rather than popping whole 8x8 tiles.
                for (int dy = 0; dy < 8; ++dy) {
                    for (int dx = 0; dx < 8; ++dx) {
                        const auto n = hash(static_cast<std::uint32_t>(
                            (x + dx) * 131 + (y + dy) * 719 + 0xBEEF));
                        if (static_cast<double>(n & 255U) / 255.0 < amount) {
                            canvas.putPixel(x + dx, y + dy);
                        }
                    }
                }
            }
        }
    }
}

void FrostGreeterAnimation::drawWhiteout(
    NanoCanvas<128, 64, 1>& canvas) const {
    canvas.setColor(0xFFFF);
    canvas.fillRect(0, 0, screen_width - 1, screen_height - 1);

    // First: a silent pure-white flash. Then dark cracks radiate outward.
    const double grow = smoothstep((elapsed_ - 2.00) / 0.25);
    const double dissolve = 1.0 - smoothstep((elapsed_ - 2.34) / 0.15);
    const double opacity = std::min(grow, dissolve);
    if (opacity <= 0.0) return;

    canvas.setColor(0x0000);
    for (int i = 0; i < 8; ++i) {
        const double a = i * 2.0 * pi / 8.0 - pi / 2.0;
        const double bend = (i % 2 == 0 ? 0.19 : -0.17);
        const Point start{cx, cy};
        const Point joint = ray(cx, cy, a, 17);
        const Point mid = ray(cx, cy, a + bend, 36);
        const Point tip = ray(cx, cy, a + bend * 1.7, 79);
        const double reach = grow;

        fractureLine(canvas, start, joint, reach, opacity, 0xA1U + i);
        fractureLine(canvas, joint, mid,
                     clamp01((reach - 0.22) / 0.78), opacity, 0xB2U + i);
        fractureLine(canvas, mid, tip,
                     clamp01((reach - 0.50) / 0.50), opacity, 0xC3U + i);

        // A few secondary branches help the cracks look like broken ice.
        if (i % 2 == 0) {
            const Point branch = ray(mid.x, mid.y, a + 0.9, 12);
            fractureLine(canvas, mid, branch,
                         clamp01((reach - 0.60) / 0.40),
                         opacity * 0.85, 0xD4U + i);
        }
    }
}

void FrostGreeterAnimation::drawScene(
    NanoCanvas<128, 64, 1>& canvas) const {
    if (elapsed_ >= whiteout_start_) {
        drawWhiteout(canvas);
        return;
    }

    drawSnow(canvas);
    drawCrystal(canvas);
    if (elapsed_ >= 1.04) drawStorm(canvas);
    if (elapsed_ >= storm_start_) {
        const double p = clamp01((elapsed_ - storm_start_) /
                                 (whiteout_start_ - storm_start_));
        drawIceMask(canvas, p);
    }
}

double FrostGreeterAnimation::revealThreshold(int col, int row) const {
    return blockThreshold(col, row);
}
