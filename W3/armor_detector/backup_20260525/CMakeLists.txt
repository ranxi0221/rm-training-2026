cmake_minimum_required(VERSION 3.10)
project(armor_detector)

set(CMAKE_CXX_STANDARD 11)

find_package(OpenCV REQUIRED)

# 算法库
add_library(armor_lib STATIC src/armor_detector_lib.cpp)
target_include_directories(armor_lib PUBLIC include)
target_link_libraries(armor_lib ${OpenCV_LIBS})

# 可执行文件
add_executable(image_demo app/image_demo.cpp)
target_link_libraries(image_demo armor_lib ${OpenCV_LIBS})

add_executable(video_demo app/video_demo.cpp)
target_link_libraries(video_demo armor_lib ${OpenCV_LIBS})