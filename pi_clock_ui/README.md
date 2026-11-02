# PI Clock UI

Standalone C++20 display module for PI Clock, built with **CMake and lcdgfx**.

Implements digital and analog clock screens, seasonal startup animations, screen transitions, and an experimental timer interface.

## Dependencies

- C++20 compiler
- CMake
- [lcdgfx](https://github.com/lexus2k/lcdgfx)
- SDL2 (for desktop emulation)

## SDL2 Emulator

On Ubuntu/Debian, install the required dependencies:

```bash
sudo apt install cmake build-essential pkg-config libsdl2-dev
```

From the repository root, make sure lcdgfx is cloned into `lcdgfx/`, then build:

```bash
cmake -S pi_clock_ui -B build-sdl -DPI_CLOCK_UI_EMULATOR=ON
cmake --build build-sdl -j
./build-sdl/PI_CLOCK
```

## Hardware

The module also supports SSD1306 OLED displays connected via I²C. See the main [README](../README.md) for ROS 2 integration.