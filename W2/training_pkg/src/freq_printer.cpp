#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include <chrono>

class FreqPrinter : public rclcpp::Node {
public:
    FreqPrinter() : Node("freq_printer"), count_(0), last_time_(this->now()) {
        subscription_ = this->create_subscription<std_msgs::msg::String>(
            "chatter", 10, std::bind(&FreqPrinter::callback, this, std::placeholders::_1));
        timer_ = this->create_wall_timer(
            std::chrono::seconds(1),
            std::bind(&FreqPrinter::print_freq, this));
    }

private:
    void callback(const std_msgs::msg::String::SharedPtr msg) {
        count_++;
    }
    void print_freq() {
        auto now = this->now();
        auto elapsed = (now - last_time_).seconds();
        if (elapsed >= 1.0) {
            double freq = count_ / elapsed;
            RCLCPP_INFO(this->get_logger(), "接收频率: %.2f Hz", freq);
            count_ = 0;
            last_time_ = now;
        }
    }
    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr subscription_;
    rclcpp::TimerBase::SharedPtr timer_;
    size_t count_;
    rclcpp::Time last_time_;
};

int main(int argc, char * argv[]) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<FreqPrinter>());
    rclcpp::shutdown();
    return 0;
}