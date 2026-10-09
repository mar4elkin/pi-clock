cmake_minimum_required(VERSION 3.16)
project(pi_clock LANGUAGES C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# lcdgfx находится рядом с pi-clock
set(LCDGFX_ROOT
    "${CMAKE_CURRENT_SOURCE_DIR}/../lcdgfx"
    CACHE PATH "Path to lcdgfx"
)

if(NOT EXISTS "${LCDGFX_ROOT}/CMakeLists.txt")
    message(FATAL_ERROR "lcdgfx not found: ${LCDGFX_ROOT}")
endif()

option(PI_CLOCK_EMULATOR "Use SDL2 emulator" OFF)

# Подключаем lcdgfx
add_subdirectory(
    "${LCDGFX_ROOT}"
    "${CMAKE_CURRENT_BINARY_DIR}/lcdgfx-build"
    EXCLUDE_FROM_ALL
)

target_include_directories(lcdgfx PUBLIC
    "${LCDGFX_ROOT}/src"
)

# Наше приложение
add_executable(pi_clock
    src/main.cpp

    src/screen/clock_display_screen.cpp
    src/screen/analog_clock_screen.cpp

    src/animation/transitions/wipe_transition.cpp

    src/animation/greeters/seasonal/seasonal_greeter_animation.cpp
    src/animation/greeters/seasonal/greeter_drawing.cpp
    src/animation/greeters/seasonal/frost_greeter_animation.cpp
    src/animation/greeters/seasonal/spring_greeter_animation.cpp
    src/animation/greeters/seasonal/summer_greeter_animation.cpp
    src/animation/greeters/seasonal/autumn_greeter_animation.cpp
)

target_include_directories(pi_clock PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_link_libraries(pi_clock PRIVATE lcdgfx)

# SDL нужен только для эмулятора
if(PI_CLOCK_EMULATOR)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(
        SDL2 REQUIRED IMPORTED_TARGET sdl2
    )

    file(GLOB SDL_EMULATOR_SOURCES CONFIGURE_DEPENDS
        "${LCDGFX_ROOT}/tools/sdl/*.c"
    )

    add_library(lcdgfx_sdl STATIC
        ${SDL_EMULATOR_SOURCES}
    )

    target_include_directories(lcdgfx_sdl PUBLIC
        "${LCDGFX_ROOT}/tools/sdl"
    )

    target_link_libraries(lcdgfx_sdl PUBLIC
        PkgConfig::SDL2
    )

    target_compile_definitions(lcdgfx PUBLIC
        SDL_EMULATION
    )

    target_include_directories(lcdgfx PRIVATE
        "${LCDGFX_ROOT}/tools/sdl"
    )

    target_link_libraries(pi_clock PRIVATE
        lcdgfx_sdl
    )
endif()