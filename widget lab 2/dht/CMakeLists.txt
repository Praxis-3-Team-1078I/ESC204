cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)
project(dht C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()

add_executable(dht
  dht.cpp
)
pico_enable_stdio_usb(dht 1)
pico_enable_stdio_uart(dht 0)
pico_add_extra_outputs(dht)
target_link_libraries(dht pico_stdlib hardware_i2c)