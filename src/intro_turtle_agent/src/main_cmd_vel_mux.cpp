#include "rclcpp/rclcpp.hpp"
#include "intro_turtle_agent/cmd_vel_mux.hpp"

int main(int argc, char ** argv) {
    rclcpp::init(argc, argv);
    auto node = std::make_shared<turtle_agent::CmdVelMux>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}