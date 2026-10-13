#pragma once

#include <deque>
#include <memory>
#include <utility>

#include "screen/screen.hpp"

template <typename... Screens>
class ScreenController {
public:
    ScreenController() {
        (screens_.push_back(
            std::make_unique<Screens>()
        ), ...);
    }

    ScreenController(std::unique_ptr<Screens>... screens) {
        (screens_.push_back(std::move(screens)), ...);
    }

    void begin() {
        if (!screens_.empty()) {
            screens_.front()->onEnter();
        }
    }

    void update(const RenderContext& context) {
        if (!screens_.empty()) {
            screens_.front()->update(context);
        }
    }

    void render(NanoCanvas<128, 64, 1>& canvas) {
        if (!screens_.empty()) {
            screens_.front()->render(canvas);
        }
    }

    void handleInput(InputAction action) {
        if (screens_.empty()) {
            return false;
        }

        return screens_.front()->handleInput(action);

        if (screens_.front()->handleInput(action)) { return; }
        
        switch (action)
        {
            case InputAction::Next:
                nextScreen();
                break;
            case InputAction::Previous:        
            case InputAction::Select:
            case InputAction::Back:
                break;
                
            default:
                break;
        }
    }

    void nextScreen() {
        if (screens_.size() <= 1) {
            return;
        }

        screens_.front()->onExit();

        screens_.push_back(std::move(screens_.front()));
        screens_.pop_front();

        screens_.front()->onEnter();
    }

private:
    std::deque<std::unique_ptr<BaseScreen>> screens_;
};