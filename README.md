# PI Clock

A Raspberry Pi clock project built with **C++20 and ROS 2**, designed for a 128×64 SSD1306 OLED display.

The project includes digital and analog clock faces, system monitoring, animated screen transitions, and seasonal startup animations. Timer functionality is under development.

## Project Structure

- `pi_clock_ui/` — display rendering, screens, and animations
- `system_monitor/` — Linux system metrics
- `ros2_ws/` — ROS 2 display and input nodes
- `common/` — shared data structures

## Getting Started

**Requirements:** ROS 2 Jazzy, CMake, C++20 compiler, and [lcdgfx](https://github.com/lexus2k/lcdgfx).

Clone lcdgfx into the project root:

```bash
git clone https://github.com/lexus2k/lcdgfx.git lcdgfx
```

Build and run the ROS 2 workspace:

```bash
source /opt/ros/jazzy/setup.bash
cd ros2_ws
colcon build --symlink-install
source install/setup.bash
ros2 run pi_clock_display display_node
```

To request a screen change from another terminal with the same ROS 2 environment sourced:

```bash
ros2 topic pub --once /pi_clock/next_screen std_msgs/msg/Empty "{}"
```

## Documentation

- [UI module](pi_clock_ui/README.md)
- [Contributing](CONTRIBUTING.md)
- [MIT License](LICENSE)