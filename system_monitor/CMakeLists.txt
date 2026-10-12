
cmake_minimum_required(VERSION 3.16)

project(pi_clock_system_monitor LANGUAGES CXX)

add_library(pi_clock_system_monitor STATIC
    src/system_monitor.cpp
)

target_compile_features(pi_clock_system_monitor
    PUBLIC cxx_std_20
)

target_include_directories(pi_clock_system_monitor
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/../common/include"
)
