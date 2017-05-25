cmake_minimum_required(VERSION 3.7)
project(OrangePi0_i2c)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES common.c common.h gpio_lib.c gpio_lib.h I2c.cpp I2c.h I2cFactory.cpp I2cFactory.h II2c.h )
add_library(OrangePi0_i2c SHARED ${SOURCE_FILES})
