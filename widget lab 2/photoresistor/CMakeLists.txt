cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)
project(photoresistor C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()

add_executable(photoresistor
  photoresistor.cpp
)
pico_enable_stdio_usb(photoresistor 1)
pico_enable_stdio_uart(photoresistor 0)
pico_add_extra_outputs(photoresistor)
target_link_libraries(photoresistor pico_stdlib hardware_adc)