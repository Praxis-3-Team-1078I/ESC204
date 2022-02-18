cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)
project(pwm_led C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()

add_executable(pwm_led
  pwm_led.cpp
)
pico_enable_stdio_usb(pwm_led 1)
pico_enable_stdio_uart(pwm_led 0)
pico_add_extra_outputs(pwm_led)
target_link_libraries(pwm_led pico_stdlib hardware_pwm)