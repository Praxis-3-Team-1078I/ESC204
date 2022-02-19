cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)
project(pwm_motor C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()

add_executable(pwm_motor
  pwm_motor.cpp
)
pico_enable_stdio_usb(pwm_motor 1)
pico_enable_stdio_uart(pwm_motor 0)
pico_add_extra_outputs(pwm_motor)
target_link_libraries(pwm_motor pico_stdlib hardware_pwm)