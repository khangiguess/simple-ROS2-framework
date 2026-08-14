#ifndef INTRO_TURTLE_AGENT__TURTLE_AGENT_HPP_
#define INTRO_TURTLE_AGENT__TURTLE_AGENT_HPP_

#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/twist.hpp>


namespace turtle_agent{

class TurtleAgent : public rclcpp::Node{
    public:
        TurtleAgent();

    private:
        // Callback function declaration for incoming velocity commands
        void on_cmd_vel(const geometry_msgs::msg::Twist::SharedPtr msg);

        // ROS2 communication objects (must be stored as class members to stay alive)
        rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr sub_cmd_vel_;
        rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr pub_turtle_cmd_vel_;
};
}
#endif  // INTRO_TURTLE_AGENT__TURTLE_AGENT_HPP_