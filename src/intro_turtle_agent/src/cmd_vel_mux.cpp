#include "intro_turtle_agent/cmd_vel_mux.hpp"
#include <chrono>

namespace turtle_agent {

CmdVelMux::CmdVelMux() : rclcpp::Node("cmd_vel_mux") {
    // 1. Declare Parameters with default values
    // This allows users to change behavior via launch files or CLI without recompiling
    this->declare_parameter<double>("timeout", 1.0);
    this->declare_parameter<double>("publish_rate", 20.0);
    this->declare_parameter<bool>("default_auto_mode", true);

    // Read the parameters
    timeout_ = this->get_parameter("timeout").as_double();
    publish_rate_ = this->get_parameter("publish_rate").as_double();
    auto_mode_ = this->get_parameter("default_auto_mode").as_bool();

    // Initialize times
    last_auto_time_ = this->now();
    last_manual_time_ = this->now();

    // 2. Subscribers
    sub_auto_ = this->create_subscription<geometry_msgs::msg::Twist>(
        "auto_cmd_vel", 10, std::bind(&CmdVelMux::on_auto, this, std::placeholders::_1));
    sub_manual_ = this->create_subscription<geometry_msgs::msg::Twist>(
        "manual_cmd_vel", 10, std::bind(&CmdVelMux::on_manual, this, std::placeholders::_1));

    // 3. Publisher
    pub_cmd_vel_ = this->create_publisher<geometry_msgs::msg::Twist>("cmd_vel", 10);

    // 4. Service Server
    // std_srvs/srv/SetBool is a standard ROS2 service: Request has a bool 'data', Response has 'success' and 'message'
    srv_set_auto_mode_ = this->create_service<std_srvs::srv::SetBool>(
        "set_auto_mode",
        std::bind(&CmdVelMux::on_set_auto_mode, this, std::placeholders::_1, std::placeholders::_2));

    // 5. Timer
    // Instead of publishing immediately when a message arrives, we publish at a fixed rate.
    // This ensures a steady stream of commands and acts as a deadman switch.
    std::chrono::milliseconds period(static_cast<int>(1000.0 / publish_rate_));
    timer_ = this->create_wall_timer(
        period, std::bind(&CmdVelMux::tick, this));

    RCLCPP_INFO(this->get_logger(), "CmdVelMux ready. Auto mode: %s", auto_mode_ ? "true" : "false");
}

void CmdVelMux::on_auto(const geometry_msgs::msg::Twist::SharedPtr msg) {
    last_auto_msg_ = *msg;
    last_auto_time_ = this->now();
    have_auto_msg_ = true;
}

void CmdVelMux::on_manual(const geometry_msgs::msg::Twist::SharedPtr msg) {
    last_manual_msg_ = *msg;
    last_manual_time_ = this->now();
    have_manual_msg_ = true;
}

void CmdVelMux::on_set_auto_mode(
    const std::shared_ptr<std_srvs::srv::SetBool::Request> request,
    std::shared_ptr<std_srvs::srv::SetBool::Response> response) {
    
    auto_mode_ = request->data;
    response->success = true;
    response->message = auto_mode_ ? "Autonomous mode active" : "Manual override active";
    RCLCPP_INFO(this->get_logger(), "%s", response->message.c_str());
}

void CmdVelMux::tick() {
    rclcpp::Time now = this->now();
    geometry_msgs::msg::Twist out; // Defaults to all zeros

    if (auto_mode_) {
        double age = (now - last_auto_time_).seconds();
        // If we have a message and it's not too old, use it. Otherwise, publish zero.
        if (have_auto_msg_ && age <= timeout_) {
            out = last_auto_msg_;
        }
    } else {
        double age = (now - last_manual_time_).seconds();
        if (have_manual_msg_ && age <= timeout_) {
            out = last_manual_msg_;
        }
    }

    pub_cmd_vel_->publish(out);
}

}  // namespace intro_turtle_agent