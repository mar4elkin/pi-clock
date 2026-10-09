#pragma once

#include "lcdgfx.h"
#include "base/render_context.hpp"

class BaseView {
public:
    virtual ~BaseView() = default;

    virtual void onEnter() {}
    virtual void update(const RenderContext& context) = 0;
    virtual void render(NanoCanvas<128, 64, 1>& canvas) = 0;
    virtual void onExit() {}
};