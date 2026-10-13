
#include <chrono>
#include <ctime>
#include <functional>
#include <memory>
#include <thread>
#include <utility>

#include "system_monitor/system_monitor.hpp"

#include "display_node.hpp"

#include "display/controller.hpp"

#include "screen/clock_display_screen.hpp"
#include "screen/analog_clock_screen.hpp"
#include "screen/system_status_screen.hpp"

#include "animation/greeters/seasonal/autumn_greeter_animation.hpp"
#include "animation/greeters/seasonal/frost_greeter_animation.hpp"
#include "animation/greeters/seasonal/spring_greeter_animation.hpp"
#include "animation/greeters/seasonal/summer_greeter_animation.hpp"

DisplayNode::DisplayNode(std::function<void()> on_next_screen)
    : rclcpp::Node("display_node"),
      on_next_screen_(std::move(on_next_screen))
{
    subscription_ = this->create_subscription<std_msgs::msg::Empty>(
        "/pi_clock/next_screen",
        10,
        [this](std_msgs::msg::Empty::ConstSharedPtr) {
            RCLCPP_INFO(
                this->get_logger(),
                "Next screen requested!"
            );

            on_next_screen_();
        }
    );
}

int main(int argc, char** argv)
{
    SystemStatus status;
    SystemMonitor monitor;

    DisplayController<
        ClockDisplayScreen,
        AnalogClockScreen,
        SystemStatusScreen
    > controller(
        std::make_unique<ClockDisplayScreen>(),
        std::make_unique<AnalogClockScreen>(),
        std::make_unique<SystemStatusScreen>(status)
    );

    rclcpp::init(argc, argv);

    const auto now = std::chrono::system_clock::now();

    const std::time_t timestamp =
        std::chrono::system_clock::to_time_t(now);

    std::tm local_time{};
    localtime_r(&timestamp, &local_time);
    
    switch (local_time.tm_mon) {
        case 11:
        case 0:  
        case 1:  
            controller.begin<FrostGreeterAnimation>();
            break;

        case 2:
        case 3:
        case 4:
            controller.begin<SpringGreeterAnimation>();
            break;

        case 5:
        case 6:
        case 7:
            controller.begin<SummerGreeterAnimation>();
            break;

        case 8:
        case 9:
        case 10:
            controller.begin<AutumnGreeterAnimation>();
            break;
    }

    auto node = std::make_shared<DisplayNode>(
        [&controller]() {
            controller.handleInput(InputAction::Next);
        }
    );

    rclcpp::executors::SingleThreadedExecutor executor;
    executor.add_node(node);

    auto previous = std::chrono::steady_clock::now();

    while (rclcpp::ok()) {
        executor.spin_some();

        status = monitor.sample();

        const auto now = std::chrono::steady_clock::now();

        const double dt =
            std::chrono::duration<double>(now - previous).count();

        previous = now;

        const auto system_now = std::chrono::system_clock::now();

        const std::time_t epoch_second =
            std::chrono::system_clock::to_time_t(system_now);

        localtime_r(&epoch_second, &local_time);

        RenderContext context{
            .local_time = local_time,
            .dt = dt
        };

        controller.update(context);
        controller.render();

        std::this_thread::sleep_for(
            std::chrono::milliseconds(33)
        );
    }

    executor.remove_node(node);
    rclcpp::shutdown();

    return 0;
}
