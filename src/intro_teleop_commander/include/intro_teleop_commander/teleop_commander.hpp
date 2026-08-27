#ifndef INTRO_TELEOP_COMMANDER__TELEOP_COMMANDER_HPP_
#define INTRO_TELEOP_COMMANDER__TELEOP_COMMANDER_HPP_

#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "std_msgs/msg/bool.hpp"
#include "std_srvs/srv/set_bool.hpp"

// We need termios.h to manipulate terminal input settings
#include <termios.h>

namespace intro_teleop_commander {

class TeleopCommander : public rclcpp::Node {
public:
    TeleopCommander();
    ~TeleopCommander();

private:
    // Polls the keyboard for keypresses
    void read_keyboard();
    
    // Toggles the override state and calls the mux service
    void set_override(bool active);
    
    // Publishes the current override state to the latched topic
    void publish_state();
    
    // Publishes a zero velocity command
    void stop_robot();

    // Parameters
    double linear_speed_;
    double angular_speed_;
    std::string set_auto_mode_service_;

    // State
    bool override_active_{false};

    // ROS2 Objects
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr pub_manual_cmd_vel_;
    rclcpp::Publisher<std_msgs::msg::Bool>::SharedPtr pub_teleop_active_;
    rclcpp::Client<std_srvs::srv::SetBool>::SharedPtr client_set_auto_mode_;
    rclcpp::TimerBase::SharedPtr timer_;

    // Terminal settings (to restore normal shell behavior when node shuts down)
    struct termios original_termios_;
    bool termios_modified_{false};
};

}  // namespace intro_teleop_commander

#endif  // INTRO_TELEOP_COMMANDER__TELEOP_COMMANDER_HPP_