# pi_clock_ui

C++20 display module for **Pi Clock**, a ROS 2-based smart clock project.

Features digital and analog clock faces, animated transitions, and seasonal startup animations.

**Stack:** C++20 · CMake · lcdgfx · SDL2

## Dependencies

### Desktop — Arch Linux / WSL

```bash
sudo pacman -S --needed git cmake base-devel pkgconf sdl2
```

Ubuntu / Debian:

```bash
sudo apt install git cmake build-essential pkg-config libsdl2-dev
```

### Install lcdgfx

Clone [lcdgfx](https://github.com/lexus2k/lcdgfx) alongside the project:

```bash
cd ~/projects
git clone https://github.com/lexus2k/lcdgfx.git
```

Expected structure:

```text
projects/
├── lcdgfx/
└── pi_clock_ui/
```

## Build & Run

### Desktop (SDL2 emulator)

```bash
cmake -S . -B build-sdl -DPI_CLOCK_EMULATOR=ON
cmake --build build-sdl -j4
./build-sdl/pi_clock
```

### Raspberry Pi (SSD1306 OLED)

Install dependencies on Raspberry Pi OS:

```bash
sudo apt update
sudo apt install git cmake build-essential i2c-tools libi2c-dev
```

Enable I²C using `sudo raspi-config` under **Interface Options → I2C**, then reboot.

Check the connected display:

```bash
sudo i2cdetect -y 1
```

Build directly on Raspberry Pi:

```bash
cmake -S . -B build-pi -DPI_CLOCK_EMULATOR=OFF
cmake --build build-pi -j2
./build-pi/pi_clock
```

SDL2 is not required for Raspberry Pi hardware mode.

## Status

Standalone display subsystem. ROS 2 integration is planned in the main Pi Clock project.