cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)
project(ultrasonic C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()

add_executable(ultrasonic
  ultrasonic.cpp
)
pico_enable_stdio_usb(ultrasonic 1)
pico_enable_stdio_uart(ultrasonic 0)
pico_add_extra_outputs(ultrasonic)
target_link_libraries(ultrasonic pico_stdlib)