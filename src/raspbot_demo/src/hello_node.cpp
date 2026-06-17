#include "rclcpp/rclcpp.hpp"

class HelloNode : public rclcpp::Node
{
public:
    HelloNode() : Node("hello_node")
    {
        RCLCPP_INFO(this->get_logger(), "HelloNode 已启动，executor 即将接管本线程");
    }
};

int main(int argc, char **argv)
{
    rclcpp::init(argc,argv);    // 解析 ROS 参数、连上 DDS
    auto node = std::make_shared<HelloNode>();
    rclcpp::spin(node);         // 阻塞在 executor 事件循环
    rclcpp::shutdown();
    return 0;
}