// Entry point that runs the node



#include "rclcpp/rclcpp.hpp"
#include "intro_turtle_agent/turtle_agent.hpp"

int main(int argc, char ** argv) {
    // 1. Initialize ROS2 communications
    rclcpp::init(argc, argv);

    // 2. Create an instance of the node and spin it
    // rclcpp::spin() blocks here, processing callbacks until you press Ctrl+C
    auto node = std::make_shared<turtle_agent::TurtleAgent>();
    rclcpp::spin(node);

    // 3. Shutdown ROS2 cleanly
    rclcpp::shutdown();
    return 0;
}