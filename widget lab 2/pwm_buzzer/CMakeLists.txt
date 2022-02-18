cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)
project(pwm_buzzer C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()

add_executable(pwm_buzzer
  pwm_buzzer.cpp
)
pico_enable_stdio_usb(pwm_buzzer 1)
pico_enable_stdio_uart(pwm_buzzer 0)
pico_add_extra_outputs(pwm_buzzer)
target_link_libraries(pwm_buzzer pico_stdlib hardware_pwm)