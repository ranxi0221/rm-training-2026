cmake_minimum_required(VERSION 3.8)
project(training_pkg)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(example_interfaces REQUIRED)   # 只需这个，不需要 rosidl_default_generators

# 添加可执行文件
add_executable(talker src/talker.cpp)
ament_target_dependencies(talker rclcpp std_msgs)

add_executable(listener src/listener.cpp)
ament_target_dependencies(listener rclcpp std_msgs)

add_executable(freq_printer src/freq_printer.cpp)
ament_target_dependencies(freq_printer rclcpp std_msgs)

add_executable(add_server src/add_server.cpp)
ament_target_dependencies(add_server rclcpp example_interfaces)   # 依赖 example_interfaces

# 安装所有目标
install(TARGETS talker listener freq_printer add_server
  DESTINATION lib/${PROJECT_NAME})

# 安装 launch 和 config 目录
install(DIRECTORY launch DESTINATION share/${PROJECT_NAME})
install(DIRECTORY config DESTINATION share/${PROJECT_NAME})

# 测试相关（可选）
if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  set(ament_cmake_copyright_FOUND TRUE)
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

# 最后一行必须是 ament_package()
ament_package()