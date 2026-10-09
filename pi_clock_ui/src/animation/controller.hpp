#pragma once

#include <algorithm>
#include <memory>
#include <utility>
#include <vector>

#include "animation/animation.hpp"

class AnimationController {
public:
    template <typename T, typename... Args>
    void play(Args&&... args) {
        auto animation = std::make_unique<T>(
            std::forward<Args>(args)...
        );

        animation->onEnter();

        pending_.push_back(std::move(animation));
    }

    void update(const RenderContext& context) {
        activatePending();

        for (auto& animation : animations_) {
            animation->update(context);
        }

        std::erase_if(animations_, [](const auto& animation) {
            if (!animation->isFinished()) {
                return false;
            }

            animation->onExit();
            return true;
        });

        activatePending();
    }

    void render(NanoCanvas<128, 64, 1>& canvas) {
        for (auto& animation : animations_) {
            animation->render(canvas);
        }
    }

private:
    void activatePending() {
        for (auto& animation : pending_) {
            animations_.push_back(std::move(animation));
        }

        pending_.clear();
    }

    std::vector<std::unique_ptr<BaseAnimation>> animations_;
    std::vector<std::unique_ptr<BaseAnimation>> pending_;
};