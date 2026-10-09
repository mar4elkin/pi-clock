#pragma once

#include "base/view.hpp"

class BaseScreen : public BaseView {
public:
    ~BaseScreen() override = default;
protected:
    static constexpr int center_x = 64;
    static constexpr int center_y = 31;
};