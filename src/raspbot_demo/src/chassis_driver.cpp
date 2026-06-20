#include "geometry_msgs/msg/twist.hpp"
#include "raspbot_demo/raspbot_board.hpp"
#include "rclcpp/rclcpp.hpp"
#include <algorithm>
#include <cmath>

class ChassisDriver : public rclcpp::Node {
public:
  ChassisDriver() : Node("chassis_driver"), board_("/dev/i2c-1", 0x2B) {
    // 订阅 /cmd_vel —— 控制指令必须 RELIABLE(呼应 M2!)
    sub_ = this->create_subscription<geometry_msgs::msg::Twist>(
        "cmd_vel", rclcpp::QoS(10).reliable(),
        std::bind(&ChassisDriver::on_cmd_vel, this, std::placeholders::_1));

    // 安全看门狗:超过 0.5s 没收到 cmd_vel 就停车
    watchdog_ = this->create_wall_timer(
        std::chrono::milliseconds(100),
        std::bind(&ChassisDriver::check_watchdog, this));
    last_cmd_time_ = this->now();

    RCLCPP_INFO(this->get_logger(), "chassis_driver 就绪,等待 /cmd_vel");
  }

private:
  void on_cmd_vel(const geometry_msgs::msg::Twist &msg) {
    last_cmd_time_ = this->now(); // 喂狗

    double vx = msg.linear.x;  // 前后
    double vy = msg.linear.y;  // 左右(麦轮特有)
    double wz = msg.angular.z; // 旋转

    // 麦轮逆运动学(你验证过的公式)
    const double k = 1.0;       // M5 标定真实值
    const double scale = 200.0; // m/s 等 → PWM 量级,先粗调
    int l1 = std::lround((vx - vy - wz * k) * scale);
    int l2 = std::lround((vx + vy - wz * k) * scale);
    int r1 = std::lround((vx + vy + wz * k) * scale);
    int r2 = std::lround((vx - vy + wz * k) * scale);

    auto clamp255 = [](int v) { return std::clamp(v, -255, 255); };
    board_.set_motor_signed(0, clamp255(l1));
    board_.set_motor_signed(1, clamp255(l2));
    board_.set_motor_signed(2, clamp255(r1));
    board_.set_motor_signed(3, clamp255(r2));
  }

  void check_watchdog() {
    // 0.5s 没新指令 → 停车(防止遥控断了车一直冲)
    if ((this->now() - last_cmd_time_).seconds() > 0.5) {
      board_.stop_all();
    }
  }

  RaspbotBoard board_;
  rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr sub_;
  rclcpp::TimerBase::SharedPtr watchdog_;
  rclcpp::Time last_cmd_time_;
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<ChassisDriver>());
  rclcpp::shutdown();   // 节点退出,board_ 析构 → stop_all() 自动停车
  return 0;
}