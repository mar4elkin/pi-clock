#include <chrono>
#include <csignal>
#include <ctime>
#include <thread>

#include "pi_clock/system_status.hpp"

#include "display/controller.hpp"

#include "screen/clock_display_screen.hpp"
#include "screen/analog_clock_screen.hpp"
#include "screen/system_status_screen.hpp"

#include "animation/greeters/seasonal/frost_greeter_animation.hpp"

namespace {
    volatile std::sig_atomic_t running = 1;

    void stop_on_signal(int) {
        running = 0;
    }
}

int main() {
    std::signal(SIGINT, stop_on_signal);
    std::signal(SIGTERM, stop_on_signal);

    SystemStatus status;

    DisplayController<
        ClockDisplayScreen,
        AnalogClockScreen,
        SystemStatusScreen
    > controller(
        std::make_unique<ClockDisplayScreen>(),
        std::make_unique<AnalogClockScreen>(),
        std::make_unique<SystemStatusScreen>(status)
    );

    controller.begin<FrostGreeterAnimation>();

    std::time_t last_second = static_cast<std::time_t>(-1);
    auto previous = std::chrono::steady_clock::now();

    while (running) {
        const auto now = std::chrono::steady_clock::now();

        const double dt =
            std::chrono::duration<double>(
                now - previous
            ).count();

        previous = now;

        const auto system_now =
            std::chrono::system_clock::now();

        const std::time_t epoch_second =
            std::chrono::system_clock::to_time_t(system_now);

        std::tm local_time{};
        localtime_r(&epoch_second, &local_time);

        RenderContext context{
            .local_time = local_time,
            .dt = dt
        };

        if (epoch_second != last_second) {
            if (last_second != static_cast<std::time_t>(-1)
                && local_time.tm_sec % 10 == 0) {
                controller.handleInput(InputAction::Next);
            }

            last_second = epoch_second;
        }

        controller.update(context);
        controller.render();

        std::this_thread::sleep_for(
            std::chrono::milliseconds(33)
        );
    }

    return 0;
}