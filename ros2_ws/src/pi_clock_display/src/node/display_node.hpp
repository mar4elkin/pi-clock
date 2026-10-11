#include <functional>
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/empty.hpp"

class DisplayNode : public rclcpp::Node {
public:
    explicit DisplayNode(std::function<void()> on_next_screen);

private:
    std::function<void()> on_next_screen_;
    rclcpp::Subscription<std_msgs::msg::Empty>::SharedPtr subscription_;
};