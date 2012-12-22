#=============================================================================#
# Author: QueezyTheGreat                                                      #
# Date:   26.04.2011                                                          #
#                                                                             #
# Description: Arduino CMake example                                          #
#                                                                             #
#=============================================================================#

set(CMAKE_MODULE_PATH    ${CMAKE_SOURCE_DIR}/../../../../../arduino-cmake/cmake/modules)  # CMake module search path
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/../../../../../arduino-cmake/cmake/toolchains/Arduino.cmake) # Arduino Toolchain
set(ARDUINO_SDK_VERSION 1.0)

cmake_minimum_required(VERSION 2.8)
#====================================================================#
#  Setup Project                                                     #
#====================================================================#
project(calibration C CXX)
find_package(Arduino 1.0 REQUIRED)

#====================================================================#
#  Settings                                                          #
#====================================================================#
set(FIRMWARE_NAME calibration)

set(${FIRMWARE_NAME}_LIBS m)
set(${FIRMWARE_NAME}_BOARD mega)               # Arduino Target board
set(${FIRMWARE_NAME}_SRCS calibration.cpp)  # Firmware sources
set(${FIRMWARE_NAME}_PORT /dev/ttyUSB0)            # Serial upload port

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../..)

# Fix for libm
#set(CMAKE_EXE_LINKER_FLAGS -lm)

#====================================================================#
#  Target generation                                                 #
#====================================================================#
generate_arduino_firmware(${FIRMWARE_NAME})
