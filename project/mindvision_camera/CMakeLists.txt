cmake_minimum_required(VERSION 3.8)
project(mindvision_camera)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(cv_bridge REQUIRED)
find_package(OpenCV REQUIRED)

# 头文件在 /usr/include/CameraApi.h（系统标准路径）
include_directories(${OpenCV_INCLUDE_DIRS})

# 库在 /usr/lib/libMVSDK.so
link_directories(/usr/lib)

add_executable(mindvision_camera_node
  src/mindvision_camera_node.cpp
)

# ✅ 链接 MVSDK（不是 MindVision_Camera！）
target_link_libraries(mindvision_camera_node
  ${OpenCV_LIBS}
  MVSDK   # ← 正确的库名
)

ament_target_dependencies(mindvision_camera_node
  rclcpp sensor_msgs cv_bridge
)

install(TARGETS mindvision_camera_node
  DESTINATION lib/${PROJECT_NAME}
)

ament_package()