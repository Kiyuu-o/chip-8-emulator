cmake_minimum_required(VERSION 3.26)
project(chip_8_emulator)

set(CMAKE_CXX_STANDARD 14)

add_executable(chip_8_emulator main.cpp
        chip8.h
        chip8.cpp)
