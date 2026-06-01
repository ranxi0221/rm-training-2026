cmake_minimum_required(VERSION 3.8)
project(armor_detector)

# ---- 依赖 ----
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(std_msgs REQUIRED)
find_package(cv_bridge REQUIRED)
find_package(OpenCV REQUIRED)

# ---- 头文件路径 ----
include_directories(include ${OpenCV_INCLUDE_DIRS})

# ---- 检测算法库 ----
add_library(armor_detector_lib
  src/armor_detector.cpp
)
ament_target_dependencies(armor_detector_lib
  OpenCV
)
target_include_directories(armor_detector_lib PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

# ---- ROS2 节点可执行文件 ----
add_executable(armor_detector_node
  src/armor_detector_node.cpp
)
ament_target_dependencies(armor_detector_node
  rclcpp sensor_msgs std_msgs cv_bridge OpenCV
)
target_link_libraries(armor_detector_node
  armor_detector_lib
)

# ---- 安装 ----
install(TARGETS
  armor_detector_lib
  armor_detector_node
  DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY include/
  DESTINATION include
)

install(DIRECTORY launch config
  DESTINATION share/${PROJECT_NAME}
)

ament_package()
