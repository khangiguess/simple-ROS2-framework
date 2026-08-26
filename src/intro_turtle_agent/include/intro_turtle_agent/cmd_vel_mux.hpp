
#ifndef TURTLE_AGENT__CMD_VEL_MUX_HPP_
#define TURTLE_AGENT__CMD_VEL_MUX_HPP_

#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "std_srvs/srv/set_bool.hpp"


namespace turtle_agent {

class CmdVelMux : public rclcpp::Node {
public:
    CmdVelMux();

private:
    // Callbacks for incoming velocities
    void on_auto(const geometry_msgs::msg::Twist::SharedPtr msg);
    void on_manual(const geometry_msgs::msg::Twist::SharedPtr msg);

    // Callback for the service that toggles auto/manual mode
    void on_set_auto_mode(
        const std::shared_ptr<std_srvs::srv::SetBool::Request> request,
        std::shared_ptr<std_srvs::srv::SetBool::Response> response);

    // Timer callback that fires at a fixed rate
    void tick();

    // Parameters
    double timeout_;
    double publish_rate_;
    bool auto_mode_;

    // Cached state (last received messages and times)
    geometry_msgs::msg::Twist last_auto_msg_;
    geometry_msgs::msg::Twist last_manual_msg_;
    rclcpp::Time last_auto_time_;
    rclcpp::Time last_manual_time_;
    bool have_auto_msg_{false};
    bool have_manual_msg_{false};

    // ROS2 objects
    rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr sub_auto_;
    rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr sub_manual_;
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr pub_cmd_vel_;
    rclcpp::Service<std_srvs::srv::SetBool>::SharedPtr srv_set_auto_mode_;
    rclcpp::TimerBase::SharedPtr timer_;
};

}  // namespace turtle_agent

#endif  // URTLE_AGENT__CMD_VEL_MUX_HPP_