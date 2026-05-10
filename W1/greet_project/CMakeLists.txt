
cmake_minimum_required(VERSION 3.10)
project(Greeter)

set(CMAKE_CXX_STANDARD 11)

include_directories(include)

add_executable(greet src/main.cpp src/greet.cpp)

target_compile_options(greet PRIVATE -Wall -Wextra)
# 添加警告选项