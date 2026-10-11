
cmake_minimum_required(VERSION 3.16)
project(PI_CLOCK_UI LANGUAGES C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(LCDGFX_ROOT
    "${CMAKE_CURRENT_SOURCE_DIR}/../lcdgfx"
    CACHE PATH "Path to lcdgfx"
)

if(NOT EXISTS "${LCDGFX_ROOT}/CMakeLists.txt")
    message(FATAL_ERROR "lcdgfx not found: ${LCDGFX_ROOT}")
endif()

# Режим SDL2-эмулятора
option(PI_CLOCK_UI_EMULATOR "Use SDL2 emulator" OFF)

# Подключение lcdgfx
add_subdirectory(
    "${LCDGFX_ROOT}"
    "${CMAKE_CURRENT_BINARY_DIR}/lcdgfx-build"
    EXCLUDE_FROM_ALL
)

target_include_directories(lcdgfx PUBLIC
    "${LCDGFX_ROOT}/src"
)

add_library(PI_CLOCK_UI STATIC
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

target_compile_features(PI_CLOCK_UI PUBLIC cxx_std_20)

target_include_directories(PI_CLOCK_UI PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_link_libraries(PI_CLOCK_UI PUBLIC
    lcdgfx
)

if(PI_CLOCK_UI_EMULATOR)
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

    target_link_libraries(PI_CLOCK_UI PUBLIC
        lcdgfx_sdl
    )
endif()

add_executable(PI_CLOCK
    src/main.cpp
)

target_link_libraries(PI_CLOCK PRIVATE
    PI_CLOCK_UI
)