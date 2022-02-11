cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)
project(thermoresistor C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()

add_executable(thermoresistor
  thermoresistor.cpp
)
pico_enable_stdio_usb(thermoresistor 1)
pico_enable_stdio_uart(thermoresistor 0)
pico_add_extra_outputs(thermoresistor)
target_link_libraries(thermoresistor pico_stdlib hardware_adc)