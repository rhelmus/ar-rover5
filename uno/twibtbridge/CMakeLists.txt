set(CMAKE_TOOLCHAIN_FILE /mnt/stuff/shared/src/arduino/arduino-cmake/cmake/ArduinoToolchain.cmake) # Arduino Toolchain
set(ARDUINO_SDK_VERSION 1.0)

cmake_minimum_required(VERSION 2.8)

project(calibration C CXX)

set(FIRMWARE_NAME twibtbridge)

set(${FIRMWARE_NAME}_BOARD uno)               # Arduino Target board
set(${FIRMWARE_NAME}_SRCS twibtbridge.cpp)  # Firmware sources
set(${FIRMWARE_NAME}_PORT /dev/ttyACM0)            # Serial upload port

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../sketches/libraries)

#====================================================================#
#  Target generation                                                 #
#====================================================================#
generate_arduino_firmware(${FIRMWARE_NAME})
