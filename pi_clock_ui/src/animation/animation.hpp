#pragma once

#include "base/view.hpp"

class BaseAnimation : public BaseView {
public:
    ~BaseAnimation() override = default;

    virtual bool isFinished() const = 0;
};