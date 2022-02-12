cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)
project(calibrate_ultrasonic C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()

add_executable(calibrate_ultrasonic
  calibrate_ultrasonic.cpp
)
pico_enable_stdio_usb(calibrate_ultrasonic 1)
pico_enable_stdio_uart(calibrate_ultrasonic 0)
pico_add_extra_outputs(calibrate_ultrasonic)
target_link_libraries(calibrate_ultrasonic pico_stdlib)