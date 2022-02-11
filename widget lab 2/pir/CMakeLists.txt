cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)
project(pir C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()

add_executable(pir
  pir.cpp
)
pico_enable_stdio_usb(pir 1)
pico_enable_stdio_uart(pir 0)
pico_add_extra_outputs(pir)
target_link_libraries(pir pico_stdlib)