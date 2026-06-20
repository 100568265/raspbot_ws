#include "rclcpp/rclcpp.hpp"
#include "raspbot_interfaces/srv/get_status.hpp"
#include <chrono>
#include <rclcpp/executors.hpp>
#include <rclcpp/future_return_code.hpp>
#include <rclcpp/node.hpp>

using namespace std::chrono_literals;

class StatusClient : public rclcpp::Node{
public:
    StatusClient() : Node("status_client"){
        client_ = this->create_client<raspbot_interfaces::srv::GetStatus>("get_status");
    }

    void send_query(){
        // 1. 先等服务端上线（最多等 5 秒）
        while(!client_->wait_for_service(1s)){
            if(!rclcpp::ok()) return;
            RCLCPP_INFO(this->get_logger(), "等待 get_status 服务上线...");
        }

        // 2. 构造请求（这次为空），异步发送
        auto req = std::make_shared<raspbot_interfaces::srv::GetStatus::Request>();
        auto future = client_->async_send_request(req);

        // 3. 关键：在 main 里用 spin_until_future_complete 等结果
        //    注意这是在【节点外部】等，不是在回调里等——所以不死锁
        if(rclcpp::spin_until_future_complete(this->get_node_base_interface(),future) == rclcpp::FutureReturnCode::SUCCESS){
            auto res = future.get();
            RCLCPP_INFO(this->get_logger(), "查询结果: state=%s uptime=%u", res->state.c_str(), res->uptime_sec);
        }else{
            RCLCPP_ERROR(this->get_logger(), "服务调用失败");
        }
    }

private:
    rclcpp::Client<raspbot_interfaces::srv::GetStatus>::SharedPtr client_;
};

int main(int argc, char** argv){
    rclcpp::init(argc,argv);
    auto node = std::make_shared<StatusClient>();
    node->send_query(); // 调一次就退出（一次性查询客户端）
    rclcpp::shutdown();
    return 0;
}
