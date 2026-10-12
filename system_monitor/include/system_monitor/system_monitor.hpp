
#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include "pi_clock/system_status.hpp"

class SystemMonitor {
public:
    SystemStatus sample();

private:
    struct CpuTicks {
        std::uint64_t idle = 0;
        std::uint64_t total = 0;
    };

    std::optional<CpuTicks> previous_cpu_ticks_;

    static std::optional<CpuTicks> read_cpu_ticks();

    std::optional<float> cpu_usage();

    static std::optional<float> cpu_temperature();
    static std::optional<float> ram_usage();

    static std::optional<std::uint64_t> uptime();
    static std::optional<std::string> ip_address();
};
