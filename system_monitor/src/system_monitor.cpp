
#include "system_monitor/system_monitor.hpp"

#include <arpa/inet.h>
#include <filesystem>
#include <fstream>
#include <ifaddrs.h>
#include <net/if.h>
#include <netinet/in.h>
#include <sstream>
#include <string_view>
#include <sys/statvfs.h>
#include <sys/sysinfo.h>

namespace {

std::optional<long> read_number(
    const std::filesystem::path& path
) {
    std::ifstream file(path);
    long number = 0;

    if (file >> number) {
        return number;
    }

    return std::nullopt;
}

std::string read_first_line(
    const std::filesystem::path& path
) {
    std::ifstream file(path);
    std::string line;
    std::getline(file, line);
    return line;
}

bool is_cpu_sensor(std::string_view name) {
    return name.find("cpu") != name.npos ||
           name.find("soc") != name.npos ||
           name == "coretemp" ||
           name == "k10temp" ||
           name == "zenpower" ||
           name == "x86_pkg_temp";
}

std::optional<std::string> default_ipv4_interface() {
    std::ifstream file("/proc/net/route");
    std::string line;

    std::getline(file, line); // Header

    while (std::getline(file, line)) {
        std::istringstream row(line);

        std::string iface;
        std::string destination;
        std::string gateway;
        std::string flags_text;

        if (!(row >> iface >> destination
                  >> gateway >> flags_text)) {
            continue;
        }

        unsigned int flags = 0;
        std::istringstream flags_stream(flags_text);

        if (!(flags_stream >> std::hex >> flags)) {
            continue;
        }

        if (destination == "00000000" &&
            (flags & 0x1U)) {
            return iface;
        }
    }

    return std::nullopt;
}

} // namespace

SystemStatus SystemMonitor::sample() {
    return {
        .cpu_temperature = cpu_temperature(),
        .cpu_usage = cpu_usage(),
        .ram_usage = ram_usage(),
        .uptime_seconds = uptime(),
        .ip_address = ip_address()
    };
}

std::optional<SystemMonitor::CpuTicks>
SystemMonitor::read_cpu_ticks() {
    std::ifstream file("/proc/stat");

    std::string label;

    std::uint64_t user = 0;
    std::uint64_t nice = 0;
    std::uint64_t system = 0;
    std::uint64_t idle = 0;
    std::uint64_t iowait = 0;
    std::uint64_t irq = 0;
    std::uint64_t softirq = 0;
    std::uint64_t steal = 0;

    if (!(file >> label >> user >> nice >> system
               >> idle >> iowait >> irq >> softirq
               >> steal) || label != "cpu") {
        return std::nullopt;
    }

    return CpuTicks{
        .idle = idle + iowait,
        .total = user + nice + system + idle +
                 iowait + irq + softirq + steal
    };
}

std::optional<float> SystemMonitor::cpu_usage() {
    const auto current = read_cpu_ticks();

    if (!current) {
        return std::nullopt;
    }

    const auto previous = previous_cpu_ticks_;
    previous_cpu_ticks_ = current;

    if (!previous ||
        current->total < previous->total ||
        current->idle < previous->idle) {
        return std::nullopt;
    }

    const auto total_delta =
        current->total - previous->total;

    const auto idle_delta =
        current->idle - previous->idle;

    if (total_delta == 0 || idle_delta > total_delta) {
        return std::nullopt;
    }

    return 100.0f *
        static_cast<float>(total_delta - idle_delta) /
        static_cast<float>(total_delta);
}

std::optional<float> SystemMonitor::cpu_temperature() {
    namespace fs = std::filesystem;

    for (const auto* root : {
        "/sys/class/thermal",
        "/sys/class/hwmon"
    }) {
        std::error_code ec;

        for (fs::directory_iterator it(root, ec), end;
             !ec && it != end;
             it.increment(ec)) {

            const auto dir = it->path();
            const auto filename = dir.filename().string();

            const bool is_thermal =
                std::string_view(root) ==
                "/sys/class/thermal";

            if (is_thermal) {
                if (!filename.starts_with("thermal_zone")) {
                    continue;
                }

                if (!is_cpu_sensor(
                    read_first_line(dir / "type"))) {
                    continue;
                }
            } else {
                if (!filename.starts_with("hwmon")) {
                    continue;
                }

                if (!is_cpu_sensor(
                    read_first_line(dir / "name"))) {
                    continue;
                }
            }

            const auto raw = read_number(
                dir / (is_thermal ? "temp" : "temp1_input")
            );

            if (raw) {
                return static_cast<float>(*raw) / 1000.0f;
            }
        }
    }

    return std::nullopt;
}

std::optional<float> SystemMonitor::ram_usage() {
    std::ifstream file("/proc/meminfo");

    std::string line;
    std::uint64_t total = 0;
    std::uint64_t available = 0;
    bool has_available = false;

    while (std::getline(file, line)) {
        std::istringstream row(line);

        std::string key;
        std::uint64_t value = 0;

        if (!(row >> key >> value)) {
            continue;
        }

        if (key == "MemTotal:") {
            total = value;
        }

        if (key == "MemAvailable:") {
            available = value;
            has_available = true;
        }
    }

    if (total == 0 ||
        !has_available ||
        available > total) {
        return std::nullopt;
    }

    return 100.0f *
        static_cast<float>(total - available) /
        static_cast<float>(total);
}

std::optional<std::uint64_t> SystemMonitor::uptime() {
    struct sysinfo info {};

    if (sysinfo(&info) != 0 || info.uptime < 0) {
        return std::nullopt;
    }

    return static_cast<std::uint64_t>(info.uptime);
}

std::optional<std::string> SystemMonitor::ip_address() {
    struct ifaddrs* addresses = nullptr;

    if (getifaddrs(&addresses) != 0) {
        return std::nullopt;
    }

    const auto preferred = default_ipv4_interface();

    std::optional<std::string> fallback;
    std::optional<std::string> result;

    for (auto* it = addresses;
         it != nullptr;
         it = it->ifa_next) {

        if (!it->ifa_addr ||
            !it->ifa_name ||
            it->ifa_addr->sa_family != AF_INET ||
            !(it->ifa_flags & IFF_UP) ||
            (it->ifa_flags & IFF_LOOPBACK)) {
            continue;
        }

        const auto* ipv4 =
            reinterpret_cast<const sockaddr_in*>(
                it->ifa_addr
            );

        char buffer[INET_ADDRSTRLEN]{};

        if (!inet_ntop(
            AF_INET,
            &ipv4->sin_addr,
            buffer,
            sizeof(buffer))) {
            continue;
        }

        if (!fallback) {
            fallback = std::string(buffer);
        }

        if (preferred && *preferred == it->ifa_name) {
            result = std::string(buffer);
            break;
        }
    }

    freeifaddrs(addresses);

    return result ? result : fallback;
}