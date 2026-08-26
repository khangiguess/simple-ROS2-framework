#include "intro_turtle_agent/turtle_agent.hpp"

namespace turtle_agent {

TurtleAgent::TurtleAgent() : rclcpp::Node("turtle_agent") {
    // 1. Subscriber: listens to the abstract "/cmd_vel" topic.
    // We use a relative name "cmd_vel". The launch file will remap it.
    sub_cmd_vel_ = this->create_subscription<geometry_msgs::msg::Twist>(
        "cmd_vel", 10,
        std::bind(&TurtleAgent::on_cmd_vel, this, std::placeholders::_1));

    // 2. Publisher: writes to the turtlesim specific topic.
    pub_turtle_cmd_vel_ = this->create_publisher<geometry_msgs::msg::Twist>(
        "turtle_cmd_vel", 10);

    RCLCPP_INFO(this->get_logger(), "TurtleAgent driver ready.");
}

void TurtleAgent::on_cmd_vel(const geometry_msgs::msg::Twist::SharedPtr msg) {
    // Simply take the incoming message and publish it out.
    pub_turtle_cmd_vel_->publish(*msg);
}

}  // namespace intro_turtle_agent