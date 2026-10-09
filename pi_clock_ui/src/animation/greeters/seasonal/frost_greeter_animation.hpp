#pragma once

#include <array>
#include <cstdint>

#include "animation/greeters/seasonal/seasonal_greeter_animation.hpp"

class FrostGreeterAnimation final : public SeasonalGreeterAnimation {
public:
    // Same callback order as the other three greeters.
    explicit FrostGreeterAnimation(
        Callback on_finished = {}, Callback on_reveal = {});

private:
    struct Particle {
        float x = 0.0f;
        float y = 0.0f;
        float speed = 0.0f;
    };

    static constexpr int width_ = 128;
    static constexpr int height_ = 64;
    static constexpr double crystal_start_ = 0.35;
    static constexpr double storm_start_ = 1.35;
    static constexpr double whiteout_start_ = 1.95;

    static std::uint32_t hash(std::uint32_t value);
    static double blockThreshold(int col, int row);
    static double clamp01(double value);

    void resetScene() override;
    void advanceScene(double dt) override;
    void drawScene(NanoCanvas<128, 64, 1>& canvas) const override;
    double revealThreshold(int col, int row) const override;

    void drawSnow(NanoCanvas<128, 64, 1>& canvas) const;
    void drawCrystal(NanoCanvas<128, 64, 1>& canvas) const;
    void drawStorm(NanoCanvas<128, 64, 1>& canvas) const;
    void drawIceMask(NanoCanvas<128, 64, 1>& canvas,
                     double progress) const;
    void drawWhiteout(NanoCanvas<128, 64, 1>& canvas) const;

    std::array<Particle, 36> particles_{};
};
