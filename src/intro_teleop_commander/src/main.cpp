#include "rclcpp/rclcpp.hpp"
#include "intro_teleop_commander/teleop_commander.hpp"

int main(int argc, char ** argv) {
    rclcpp::init(argc, argv);
    auto node = std::make_shared<intro_teleop_commander::TeleopCommander>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}