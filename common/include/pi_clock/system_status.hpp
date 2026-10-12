
#pragma once

#include <cstdint>
#include <optional>
#include <string>

struct SystemStatus {
    std::optional<float> cpu_temperature;
    std::optional<float> cpu_usage;
    std::optional<float> ram_usage;

    std::optional<std::uint64_t> uptime_seconds;
    std::optional<std::string> ip_address;
};
