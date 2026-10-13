#pragma once

#include "base/view.hpp"
#include "base/input_action.hpp"

class BaseScreen : public BaseView {
public:
    ~BaseScreen() override = default;
    virtual bool handleInput(InputAction action) { return false; };
protected:
    static constexpr int center_x = 64;
    static constexpr int center_y = 31;
};