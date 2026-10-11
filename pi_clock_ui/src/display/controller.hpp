#pragma once

#include "lcdgfx.h"

#include "base/render_context.hpp"
#include "screen/controller.hpp"
#include "animation/controller.hpp"
#include "animation/transitions/wipe_transition.hpp"

#include "animation/greeters/seasonal/frost_greeter_animation.hpp"
#include "animation/greeters/seasonal/spring_greeter_animation.hpp"
#include "animation/greeters/seasonal/summer_greeter_animation.hpp"
#include "animation/greeters/seasonal/autumn_greeter_animation.hpp"

template <typename... Screens>
class DisplayController {
public:
    DisplayController()
        : display_(-1) {}

    template <typename T>
    void begin() {
        display_.begin();
        display_.clear();

        screen_controller_.begin();

        boot_active_ = true;

        animation_controller_.play<T>(
            [this]() {
                screen_controller_.begin();
                boot_active_ = false;
            }
        );
    }

    void update(const RenderContext& context) {
        animation_controller_.update(context);
        if (!boot_active_) {
            screen_controller_.update(context);
        }
    }

    void render() {
        canvas_.clear();
        canvas_.setColor(0xFFFF);

        if (!boot_active_) {
            screen_controller_.render(canvas_);
        }

        screen_controller_.render(canvas_);
        animation_controller_.render(canvas_);

        display_.drawCanvas(0, 0, canvas_);
    }

    void nextScreen() {
        if (boot_active_ || transition_active_) {
            return;
        }

        if constexpr (sizeof...(Screens) < 2) {
            return;
        }

        if (transition_active_) {
            return;
        }

        transition_active_ = true;

        animation_controller_.play<WipeTransition>(
            0.6,

            [this]() {
                screen_controller_.nextScreen();
            },

            [this]() {
                transition_active_ = false;
            }
        );
    }

private:
    DisplaySSD1306_128x64_I2C display_;
    NanoCanvas<128, 64, 1> canvas_;

    ScreenController<Screens...> screen_controller_;
    AnimationController animation_controller_;

    bool boot_active_ = false;
    bool transition_active_ = false;
};