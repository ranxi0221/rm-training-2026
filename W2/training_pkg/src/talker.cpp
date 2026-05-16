
#include <chrono>
#include <functional>
#include <memory>
#include <string>
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"

using namespace std::chrono_literals;

class Talker : public rclcpp::Node {
public:
    Talker() : Node("talker"), count_(0) {
        // 1. 声明参数，默认值 500（毫秒）
        this->declare_parameter<int>("talker_period_ms", 500);
        
        // 2. 读取参数值
        int period_ms = this->get_parameter("talker_period_ms").as_int();
        
        // 3. 添加范围限制：100 ~ 5000 毫秒
        if (period_ms < 100) {
            RCLCPP_WARN(this->get_logger(), "参数 talker_period_ms=%d 小于最小值100，已强制设为100", period_ms);
            period_ms = 100;
        } else if (period_ms > 5000) {
            RCLCPP_WARN(this->get_logger(), "参数 talker_period_ms=%d 大于最大值5000，已强制设为5000", period_ms);
            period_ms = 5000;
        }
        
        // 4. 使用参数值创建定时器
        publisher_ = this->create_publisher<std_msgs::msg::String>("chatter", 10);
        timer_ = this->create_wall_timer(
            std::chrono::milliseconds(period_ms),
            std::bind(&Talker::timer_callback, this));
    }

private:
    void timer_callback() {
        auto message = std_msgs::msg::String();
        message.data = "Hello ROS2! " + std::to_string(count_++);
        RCLCPP_INFO(this->get_logger(), "Publishing: '%s'", message.data.c_str());
        publisher_->publish(message);
    }
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher_;
    rclcpp::TimerBase::SharedPtr timer_;
    size_t count_;
};

int main(int argc, char * argv[]) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<Talker>());
    rclcpp::shutdown();
    return 0;
}
