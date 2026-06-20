#include "rclcpp/rclcpp.hpp"
#include "raspbot_interfaces/srv/get_status.hpp"
#include <rclcpp/logging.hpp>

class StatusServer : public rclcpp::Node{
public:
    StatusServer() : Node("status_server")
    {
        start_time_ = this->now();
        // 创建服务：服务名 "get_status"，绑定处理回调
        srv_ = this->create_service<raspbot_interfaces::srv::GetStatus>(
            "get_status",
            std::bind(&StatusServer::handle,this,std::placeholders::_1,std::placeholders::_2));
        RCLCPP_INFO(this->get_logger(),"status_server就绪，等待查询");
    }
private:
    // 服务回调：两个参数 —— 请求进(req)、应答出(res)
    void handle(const std::shared_ptr<raspbot_interfaces::srv::GetStatus::Request> req,
        std::shared_ptr<raspbot_interfaces::srv::GetStatus::Response> res)
    {
        (void)req;  // 这次请求是空的，用不到，显式忽略避免编译警告
        res->state = "IDLE";
        res->uptime_sec = (this->now() - start_time_).seconds();
        RCLCPP_INFO(this->get_logger(), "收到查询，返回 state=%s uptime=%u",res->state.c_str(),res->uptime_sec);
        // 注意：没有 return 值。填好 res 就是应答，框架负责发回去
    }

    rclcpp::Service<raspbot_interfaces::srv::GetStatus>::SharedPtr srv_;
    rclcpp::Time start_time_;
};

int main(int argc, char** argv){
    rclcpp::init(argc,argv);
    rclcpp::spin(std::make_shared<StatusServer>());
    rclcpp::shutdown();
    return 0;
}