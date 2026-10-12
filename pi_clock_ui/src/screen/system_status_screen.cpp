#include "system_status_screen.hpp"
#include <sstream>
#include <iomanip>

SystemStatusScreen::SystemStatusScreen(const SystemStatus &status) : status_(status)
{
}

void SystemStatusScreen::update(const RenderContext &context)
{
}

void SystemStatusScreen::render(NanoCanvas<128, 64, 1> &canvas)
{
    std::stringstream ss;
    canvas.setFixedFont(ssd1306xled_font6x8);

    ss << "CPU: "
        << format_metric(status_.cpu_usage, "%")
        << " | "
        << format_metric(status_.cpu_temperature, " C");

    canvas.printFixed(0, 0, ss.str().c_str());

    ss.str("");
    ss << "RAM: "
        << format_metric(status_.ram_usage, "%");
    canvas.printFixed(0, 10, ss.str().c_str());

    canvas.drawLine(0, 20, 127, 20);

    ss.str("");
    ss << "IP: "
        << status_.ip_address.value_or("N/A");
    canvas.printFixed(0, 25, ss.str().c_str());

    ss.str("");
    if (status_.uptime_seconds) {
        const auto seconds = *status_.uptime_seconds;

        ss << "UPTIME: "
        << seconds / 3600 << "h "
        << (seconds % 3600) / 60 << "m";
    } else {
        ss << "UPTIME: N/A";
    }
    canvas.printFixed(0, 35, ss.str().c_str());
}

std::string SystemStatusScreen::format_metric(const std::optional<float> &value, std::string_view unit)
{
    if (!value) {
        return "N/A";
    }

    std::ostringstream ss;

    ss << std::fixed
       << std::setprecision(1)
       << *value
       << unit;

    return ss.str();
}
