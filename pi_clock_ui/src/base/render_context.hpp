#pragma once

#include <ctime>

struct RenderContext {
    std::tm local_time;
    double dt;
};