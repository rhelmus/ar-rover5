#=============================================================================#
# Author: QueezyTheGreat                                                      #
# Date:   26.04.2011                                                          #
#                                                                             #
# Description: Arduino CMake example                                          #
#                                                                             #
#=============================================================================#

set(CMAKE_TOOLCHAIN_FILE /mnt/stuff/shared/src/arduino/arduino-cmake/cmake/ArduinoToolchain.cmake) # Arduino Toolchain
set(ARDUINO_SDK_VERSION 1.0)

cmake_minimum_required(VERSION 2.8)
#====================================================================#
#  Setup Project                                                     #
#====================================================================#
project(LSMcalib C CXX)
#find_package(Arduino 1.0 REQUIRED)

#====================================================================#
#  Settings                                                          #
#====================================================================#
set(FIRMWARE_NAME LSMcalib)

set(${FIRMWARE_NAME}_BOARD mega)               # Arduino Target board
set(${FIRMWARE_NAME}_SRCS LSMcalib.cpp)  # Firmware sources
set(${FIRMWARE_NAME}_PORT /dev/ttyUSB0)            # Serial upload port

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../../../sketches/libraries)

#====================================================================#
#  Target generation                                                 #
#====================================================================#
generate_arduino_firmware(${FIRMWARE_NAME})
