#pragma once

#include "lcdgfx.h"
#include "screen/screen.hpp"
#include "pi_clock/system_status.hpp"

class SystemStatusScreen : public BaseScreen {
public:
    explicit SystemStatusScreen(const SystemStatus& status);
    void update(const RenderContext& context) override;
    void render(NanoCanvas<128, 64, 1>& canvas) override;

protected:
    std::string format_metric(const std::optional<float>& value, std::string_view unit = "");

private:
    const SystemStatus& status_;
};