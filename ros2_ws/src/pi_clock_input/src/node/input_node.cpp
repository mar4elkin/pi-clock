#include <cstdio>
#include <iostream>
#include <memory>
#include <string>
#include "input_node.hpp"

InputNode::InputNode() : rclcpp::Node("input_node")
{
    publisher_ = this->create_publisher<std_msgs::msg::Empty>(
        "/pi_clock/next_screen",
        10
    );
}

void InputNode::nextScreen()
{
    publisher_->publish(std_msgs::msg::Empty());
}

int main(int argc, char ** argv)
{
    rclcpp::init(argc, argv);

    auto node = std::make_shared<InputNode>();

    std::string line;

    while (std::getline(std::cin, line)) {
        node->nextScreen();
    }

    rclcpp::shutdown();
    return 0;
}