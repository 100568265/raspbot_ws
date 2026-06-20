cmake_minimum_required(VERSION 3.8)
project(raspbot_demo)

add_compile_options(-Wall -Wextra -Wpedantic)

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(raspbot_interfaces REQUIRED)
find_package(rclcpp_action REQUIRED)
find_package(geometry_msgs REQUIRED)

add_executable(hello_node src/hello_node.cpp)
ament_target_dependencies(hello_node rclcpp)

add_executable(wheel_pub src/wheel_pub.cpp)
ament_target_dependencies(wheel_pub rclcpp raspbot_interfaces)

add_executable(wheel_sub src/wheel_sub.cpp)
ament_target_dependencies(wheel_sub rclcpp raspbot_interfaces)

add_executable(status_server src/status_server.cpp)
ament_target_dependencies(status_server rclcpp raspbot_interfaces)

add_executable(status_client src/status_client.cpp)
ament_target_dependencies(status_client rclcpp raspbot_interfaces)

add_executable(rotate_server src/rotate_server.cpp)
ament_target_dependencies(rotate_server rclcpp rclcpp_action raspbot_interfaces)

add_executable(chassis_driver src/chassis_driver.cpp src/raspbot_board.cpp)
ament_target_dependencies(chassis_driver rclcpp geometry_msgs)
target_include_directories(chassis_driver PRIVATE include)
target_link_libraries(chassis_driver i2c)        # ← 链接 libi2c(C 库)


install(TARGETS hello_node wheel_pub wheel_sub status_server status_client rotate_server chassis_driver
  DESTINATION lib/${PROJECT_NAME})

ament_package()
