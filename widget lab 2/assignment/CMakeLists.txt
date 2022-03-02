cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)
project(assignment C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()

add_executable(assignment
  assignment.cpp
  lib/pwm.cpp
  lib/dht.cpp
  lib/thread.cpp
)
pico_enable_stdio_usb(assignment 1)
pico_enable_stdio_uart(assignment 0)
pico_add_extra_outputs(assignment)
target_link_libraries(assignment pico_stdlib hardware_i2c hardware_pwm pico_multicore pico_sync)