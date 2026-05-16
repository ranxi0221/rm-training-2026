#include "rclcpp/rclcpp.hpp"
#include "example_interfaces/srv/add_two_ints.hpp"

class AddServer : public rclcpp::Node {
public:
    AddServer() : Node("add_server") {
        service_ = this->create_service<example_interfaces::srv::AddTwoInts>(
            "add_two_ints",
            std::bind(&AddServer::handle, this, std::placeholders::_1, std::placeholders::_2));
    }
private:
    void handle(const std::shared_ptr<example_interfaces::srv::AddTwoInts::Request> req,
                std::shared_ptr<example_interfaces::srv::AddTwoInts::Response> res) {
        res->sum = req->a + req->b;
        RCLCPP_INFO(this->get_logger(), "收到请求: %ld + %ld = %ld", req->a, req->b, res->sum);
    }
    rclcpp::Service<example_interfaces::srv::AddTwoInts>::SharedPtr service_;
};

int main(int argc, char * argv[]) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<AddServer>());
    rclcpp::shutdown();
    return 0;
}
