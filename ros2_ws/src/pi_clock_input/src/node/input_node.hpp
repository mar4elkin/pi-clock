#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/empty.hpp"

class InputNode : public rclcpp::Node {
public:
    InputNode();
    void nextScreen();
    
private:
    rclcpp::Publisher<std_msgs::msg::Empty>::SharedPtr publisher_;
};