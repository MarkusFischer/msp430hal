cmake_minimum_required(VERSION 3.19)
project(msp430hal)

add_library(msp430hal INTERFACE include/msp430hal/util/math.h include/msp430hal/multitasking/interrupt_guard.h)
add_library(msp430hal::msp430hal ALIAS msp430hal)

target_include_directories(msp430hal INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>)