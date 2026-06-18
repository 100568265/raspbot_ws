#include "rclcpp/rclcpp.hpp"
#include "raspbot_interfaces/msg/wheel_speed.hpp"

class WheelSub : public rclcpp::Node
{
public:
    WheelSub() : Node("wheel_sub")
    {
        rclcpp::QoS qos(10);
        qos.best_effort();
        sub_ = this->create_subscription<raspbot_interfaces::msg::WheelSpeed>(
            "wheel_speed", qos,
            std::bind(&WheelSub::on_msg,this,std::placeholders::_1));
        RCLCPP_INFO(this->get_logger(), "wheel_sub 启动，等WheelSpeed");
    }

private:
    void on_msg(const raspbot_interfaces::msg::WheelSpeed & msg)
    {
        RCLCPP_INFO(this->get_logger(), "收到: FL=%.2f RR=%.2f @ stamp %d.%09u",
                msg.front_left, msg.rear_right,
                msg.header.stamp.sec, msg.header.stamp.nanosec);
    }

    rclcpp::Subscription<raspbot_interfaces::msg::WheelSpeed>::SharedPtr sub_;
};

int main(int argc, char** argv)
{
    rclcpp::init(argc,argv);
    rclcpp::spin(std::make_shared<WheelSub>());
    rclcpp::shutdown();
    return 0;
}