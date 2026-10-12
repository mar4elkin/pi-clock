sudo apt install libgpiod-dev gpiod pkg-config

sh setup for new terminal
source /opt/ros/jazzy/setup.bash
source ~/projects/pi-clock/ros2_ws/install/setup.bash

export RMW_IMPLEMENTATION=rmw_cyclonedds_cpp
export ROS_DOMAIN_ID=42
export ROS_AUTOMATIC_DISCOVERY_RANGE=LOCALHOST

ros2 run pi_clock_display display_node
ros2 topic pub --once     /pi_clock/next_screen     std_msgs/msg/Empty "{}"

sh rebuild ros2 src with emu
colcon build --symlink-install --cmake-args -DPI_CLOCK_UI_EMULATOR=ON