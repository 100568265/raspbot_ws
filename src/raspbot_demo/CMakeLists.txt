cmake_minimum_required(VERSION 3.8)
project(raspbot_demo)

add_compile_options(-Wall -Wextra -Wpedantic)

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(raspbot_interfaces REQUIRED)

add_executable(wheel_pub src/wheel_pub.cpp)
ament_target_dependencies(wheel_pub rclcpp raspbot_interfaces)

add_executable(wheel_sub src/wheel_sub.cpp)
ament_target_dependencies(wheel_sub rclcpp raspbot_interfaces)

add_executable(hello_node src/hello_node.cpp)
ament_target_dependencies(hello_node rclcpp)

install(TARGETS hello_node wheel_pub wheel_sub
  DESTINATION lib/${PROJECT_NAME})

ament_package()
