#include "rclcpp/rclcpp.hpp"
#include "rclcpp_action/rclcpp_action.hpp"
#include "raspbot_interfaces/action/rotate.hpp"
#include <memory>
#include <rclcpp/node.hpp>
#include <thread>

class RotateServer : public rclcpp::Node{
public:
    using Rotate = raspbot_interfaces::action::Rotate;
    using GoalHandle = rclcpp_action::ServerGoalHandle<Rotate>;

    RotateServer() : Node("rotate_server")
    {
        // 创建 action 服务端：动作名 "rotate"，绑定三个回调
        server_ = rclcpp_action::create_server<Rotate>(
            this,"rotate",
            std::bind(&RotateServer::handle_goal, this, std::placeholders::_1, std::placeholders::_2),
            std::bind(&RotateServer::handle_cancel, this, std::placeholders::_1),
            std::bind(&RotateServer::handle_accepted, this, std::placeholders::_1));
        
        RCLCPP_INFO(this->get_logger(), "rotate_server就绪");
    }

private:
    // ① 收到 goal：决定接受/拒绝。executor 调，必须秒回
    rclcpp_action::GoalResponse handle_goal(
        const rclcpp_action::GoalUUID & /*uuid*/,
        std::shared_ptr<const Rotate::Goal> goal)
    {
        RCLCPP_INFO(this->get_logger(), "收到目标：转 %.1f 度", goal->target_angle_deg);
        if(std::abs(goal->target_angle_deg) > 360.0){
            RCLCPP_WARN(this->get_logger(), "角度过大，拒绝");
            return rclcpp_action::GoalResponse::REJECT;
        }
        return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;
    }

    // ③ 收到取消请求：同意/拒绝。executor 调，秒回
    rclcpp_action::CancelResponse handle_cancel(const std::shared_ptr<GoalHandle> /*gh*/)
    {
        RCLCPP_INFO(this->get_logger(), "收到取消请求，同意");
        return rclcpp_action::CancelResponse::ACCEPT;
    }

    // ② 目标已接受：启动执行。关键——另起线程跑 execute，立刻返回
    void handle_accepted(const std::shared_ptr<GoalHandle> gh)
    {
        std::thread{std::bind(&RotateServer::execute, this, gh)}.detach();
    }

    // 真正干活，跑在独立线程，不占 executor
    void execute(const std::shared_ptr<GoalHandle> gh)
    {
        const auto goal = gh->get_goal();
        auto feedback = std::make_shared<Rotate::Feedback>();
        auto result = std::make_shared<Rotate::Result>();

        rclcpp::Rate rate(10);  // 10Hz，每 100ms 一步
        double current = 0.0;
        const double step = goal->target_angle_deg / 20.0;  // 假装 2 秒转完(20 步)

        while(std::abs(current) < std::abs(goal->target_angle_deg) && rclcpp::ok()){
            // 每一步先检查：客户端要取消吗？
            if(gh->is_canceling()){
                result->actual_angle_deg = current;
                result->success = false;
                gh->canceled(result);               // 标记为已取消
                RCLCPP_INFO(this->get_logger(), "已取消，停在 %.1f 度", current);
                return;
            }
            current += step;                        // 假装转动(M4 换成读真实角度)
            feedback->current_angle_deg = current;
            gh->publish_feedback(feedback);         // 推送进度
            RCLCPP_INFO(this->get_logger(), "转动中... %.1f 度", current);
            rate.sleep();
        }

        result->actual_angle_deg = current;
        result->success = true; 
        gh->succeed(result);                        // 标记成功 + 返回结果
        RCLCPP_INFO(this->get_logger(), "完成，最终 %.1f 度", current);
    }
        
    rclcpp_action::Server<Rotate>::SharedPtr server_;
};

int main(int argc, char** argv){
    rclcpp::init(argc,argv);
    rclcpp::spin(std::make_shared<RotateServer>());
    rclcpp::shutdown();
    return 0;
}