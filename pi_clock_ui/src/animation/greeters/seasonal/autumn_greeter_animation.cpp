#include "animation/greeters/seasonal/autumn_greeter_animation.hpp"
#include "animation/greeters/seasonal/greeter_drawing.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <numbers>

using namespace greeter_detail;

namespace {
constexpr double pi = std::numbers::pi;
constexpr double kWhiteoutStart = 2.03;
constexpr double kRevealStart = 2.55; // Matches SeasonalGreeterAnimation's default.

struct TwigLeaf {
    int x, y;
    double angle;
    double release;
    int size;
};

constexpr std::array<TwigLeaf, 8> kTwigLeaves{{
    {74, 7,  -0.9, 0.63, 4},
    {80, 18,  0.5, 0.82, 4},
    {111, 7, -0.5, 0.76, 4},
    {105, 17, 0.7, 1.06, 3},
    {69, 43,  1.8, 0.91, 5},
    {83, 47, -1.1, 1.18, 4},
    {112, 20, 0.8, 1.28, 3},
    {88, 10, -0.9, 1.02, 4},
}};

void drawLeaf(Canvas& c, int x, int y, double angle, int size) {
    if (x < size + 2 || x > 125 - size ||
        y < size + 2 || y > 61 - size) return;

    const Point tip   = polar(x, y, angle, size);
    const Point tail  = polar(x, y, angle + pi, size);
    const Point left  = polar(x, y, angle + pi / 2, std::max(2, size / 2));
    const Point right = polar(x, y, angle - pi / 2, std::max(2, size / 2));
    const Point stem  = polar(x, y, angle + pi, size + 2);

    line(c, tip.x, tip.y, left.x, left.y);
    line(c, left.x, left.y, tail.x, tail.y);
    line(c, tail.x, tail.y, right.x, right.y);
    line(c, right.x, right.y, tip.x, tip.y);
    line(c, tip.x, tip.y, stem.x, stem.y); // Leaf vein and stalk.
}

void growLine(Canvas& c, int x0, int y0, int x1, int y1,
              double time, double start, double duration) {
    const double p = smooth((time - start) / duration);
    if (p <= 0.0) return;
    line(c, x0, y0,
         iround(x0 + (x1 - x0) * p),
         iround(y0 + (y1 - y0) * p));
}

void drawTree(Canvas& c, double t) {
    // The branch grows from the lower-right instead of appearing instantly.
    growLine(c, 127, 63, 117, 51, t, 0.02, 0.18);
    growLine(c, 117, 51, 104, 43, t, 0.11, 0.19);
    growLine(c, 104, 43,  92, 28, t, 0.19, 0.23);
    growLine(c,  92, 28,  80, 18, t, 0.30, 0.17);
    growLine(c,  80, 18,  74,  7, t, 0.40, 0.18);
    growLine(c,  92, 28, 105, 17, t, 0.37, 0.18);
    growLine(c, 105, 17, 111,  7, t, 0.46, 0.14);
    growLine(c, 104, 43,  83, 47, t, 0.28, 0.21);
    growLine(c,  83, 47,  69, 43, t, 0.39, 0.18);
    growLine(c, 117, 51, 119, 30, t, 0.32, 0.22);
    growLine(c, 119, 30, 112, 20, t, 0.48, 0.16);
    growLine(c,  92, 28,  88, 10, t, 0.39, 0.18);

    // A secondary hairline gives depth without filling the tiny screen.
    growLine(c, 118, 53, 107, 45, t, 0.24, 0.22);
}

void drawBranchLeaves(Canvas& c, double t) {
    for (std::size_t i = 0; i < kTwigLeaves.size(); ++i) {
        const auto& l = kTwigLeaves[i];
        if (t < 0.45 + 0.03 * i) continue;

        const double sway = 0.12 * std::sin(9.0 * t + i * 1.6);
        if (t < l.release) {
            drawLeaf(c, l.x, l.y, l.angle + sway, l.size);
            continue;
        }

        // Each leaf starts exactly where it was attached. No teleporting.
        const double age = t - l.release;
        const double vx = 25.0 + i * 3.8;
        const double x = l.x - vx * age - 17.0 * age * age;
        const double y = l.y + (4.0 + i % 3 * 2.0) *
                            std::sin(age * 6.0 + i * 0.4) * smooth(age * 2.5)
                            + (static_cast<int>(i % 3) - 1) * age * 4.0;
        drawLeaf(c, iround(x), iround(y),
                 l.angle - age * (3.0 + i * 0.4), l.size);
    }
}

void drawWind(Canvas& c, double t) {
    const double rise = smooth((t - 0.54) / 0.75);
    const double fall = 1.0 - smooth((t - 2.02) / 0.48);
    const double strength = rise * fall;
    if (strength <= 0.0) return;

    for (int i = 0; i < 13; ++i) {
        const double x = std::fmod(135.0 + 150.0 * noise(i, 4, 9)
                            - (t - 0.54) * (36.0 + 7.0 * (i % 4)) + 400.0,
                            166.0) - 9.0;
        const int y = 5 + iround(noise(i, 2, 6) * 53.0 +
                                3.0 * std::sin(t * 4.0 + i));
        const int length = 3 + iround(strength * (5.0 + 2.0 * (i % 5)));
        const int xi = iround(x);
        if (xi - length < 1 || xi > 126 || y < 3 || y > 60) continue;
        line(c, xi, y, xi - length / 2, y - (i % 2));
        line(c, xi - length / 2, y - (i % 2), xi - length, y);
    }

    // Leaves carried in from the right side, beyond the ones from our tree.
    for (int i = 0; i < 19; ++i) {
        const double born = 0.55 + (i % 7) * 0.13 + (i / 7) * 0.22;
        const double age = t - born;
        if (age <= 0.0) continue;
        const double x = 134.0 + 28.0 * noise(i, 9, 3)
                         - age * (45.0 + 24.0 * noise(i, 5, 2));
        const double y = 5.0 + noise(i, 8, 11) * 50.0
                         + (3.0 + 2.0 * (i % 3)) * std::sin(age * 5.0 + i);
        drawLeaf(c, iround(x), iround(y), -age * 3.0 + i, 3 + i % 3);
    }
}

void drawVortex(Canvas& c, double t) {
    const double intensity = smooth((t - 1.22) / 0.72)
                           * (1.0 - smooth((t - 2.05) / 0.43));
    if (intensity <= 0.0) return;

    constexpr int cx = 52;
    constexpr int cy = 31;
    const int count = 5 + iround(intensity * 17.0);

    for (int i = 0; i < count; ++i) {
        const double a = i * 2.39996323 - t * (3.3 + (i % 3) * 0.15);
        const double radius = (4.0 + 24.0 * ((i + 1.0) / count))
                            * (0.42 + 0.58 * intensity);
        const Point p = polar(cx, cy, a, radius);
        if ((i % 3) == 0) {
            drawLeaf(c, p.x, p.y, a + pi * 0.4, 3 + i % 2);
        } else {
            const Point q = polar(p.x, p.y, a + pi / 2, 2 + i % 3);
            line(c, p.x, p.y, q.x, q.y);
        }
    }

    // Short broken spiral arcs: more readable than a dense circular blur.
    for (int ring = 0; ring < 2; ++ring) {
        const double r = (12.0 + 13.0 * ring) * intensity;
        for (int j = 0; j < 12; ++j) {
            if ((j + ring) % 4 == 0) continue;
            const double a = j * (pi / 6.0) - t * 2.8 + ring * 0.5;
            const Point p = polar(cx, cy, a, r);
            dot(c, p.x, p.y);
        }
    }
}

void drawWhiteout(Canvas& c, double t) {
    // The final frame at 2.55s is solid white, so the base class's
    // white reveal mask can take over without a visible cut.
    const double p = smooth((t - kWhiteoutStart) /
                            (kRevealStart - kWhiteoutStart));
    if (p <= 0.0) return;

    c.setColor(0xFFFF);
    for (int y = 0; y < 64; ++y) {
        const double waviness = (1.0 - p) *
            (5.0 * std::sin(y * 0.29 + t * 13.0) +
             3.0 * (noise(y, 6, 13) - 0.5));
        const int edge = iround(133.0 - 167.0 * p +
                                (y - 32) * 0.27 + waviness);
        if (edge < 128) {
            c.fillRect(std::max(0, edge), y, 127, y);
        }
    }
}
} // namespace

void AutumnGreeterAnimation::drawScene(Canvas& c) const {
    // Rendered only before reveal_start (2.55s). No text, no empty gap.
    const double t = elapsed_;
    drawTree(c, t);
    drawBranchLeaves(c, t);
    drawWind(c, t);
    drawVortex(c, t);
    drawWhiteout(c, t);
}

double AutumnGreeterAnimation::revealThreshold(int col, int row) const {
    // Upper-right pixels appear first, as the gust uncovers the clock.
    const double rightToLeft = (15.0 - col) / 15.0;
    return clamp01(0.04 + 0.76 * rightToLeft +
                   0.12 * (row / 7.0) +
                   0.08 * noise(col, row, 14));
}
