#include "raspbot_interfaces/msg/wheel_speed.hpp"
#include "rclcpp/rclcpp.hpp"

using namespace std::chrono_literals;

class WheelPub : public rclcpp::Node {
public:
  WheelPub() : Node("wheel_pub"), tick_(0) {
    rclcpp::QoS qos(10);
    qos.best_effort();
    pub_ = this->create_publisher<raspbot_interfaces::msg::WheelSpeed>("wheel_speed",qos);      // 建一个往话题 wheel_chatter 发的发布者。10是QoS队列深度(发太快来不及发时缓冲多少条)
    timer_ = this->create_wall_timer(
        500ms, std::bind(&WheelPub::on_timer,
                         this)); // 注册了一个周期性回调。executor 不再纯阻塞,每
                                 // 500ms 派发一次 on_timer。
    RCLCPP_INFO(this->get_logger(), "wheel_pub启动, 每500ms 发一次");
  }

private:
  void on_timer() {
    auto msg = raspbot_interfaces::msg::WheelSpeed();
    msg.header.stamp = this->now(); // ← 时间戳纪律：发的瞬间盖时间章
    msg.header.frame_id = "base_link"; // ← 这数据属于车体坐标系
    double v = 0.1 * tick_++;          // 假数据，递增轮速
    msg.front_left = v;
    msg.front_right = v;
    msg.rear_left = v;
    msg.rear_right = v;
    RCLCPP_INFO(this->get_logger(), "发布: FL=%.2f FR=%.2f RL=%.2f RR=%.2f",
                msg.front_left, msg.front_right, msg.rear_left, msg.rear_right);
    pub_->publish(msg); // DDS 替你做序列化 + 分发给所有订阅者。你不用管有几个订阅者、它们在哪,这就是发布订阅解耦
  }

  rclcpp::TimerBase::SharedPtr timer_;
  rclcpp::Publisher<raspbot_interfaces::msg::WheelSpeed>::SharedPtr pub_;
  size_t tick_;
};

int main(int argc, char **argv) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<WheelPub>());
  rclcpp::shutdown();
  return 0;
}