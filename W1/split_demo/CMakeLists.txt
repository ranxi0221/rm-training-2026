cmake_minimum_required(VERSION 3.10)
project(Calculator)

set(CMAKE_CXX_STANDARD 11)

include_directories(include)

add_executable(calc src/main.cpp src/calculator.cpp)
