#include "intro_teleop_commander/teleop_commander.hpp"

#include <fcntl.h>
#include <unistd.h>
#include <cstdio>

namespace intro_teleop_commander {

// --- Helper Function: Non-blocking getchar ---
// Standard getchar() waits for the Enter key. We don't want that for robot control.
// This function temporarily disables canonical mode and echo, checks for a key, 
// and restores the settings.
static int getch_nonblocking() {
    struct termios oldt, newt;
    int ch;
    
    // Get current terminal settings
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    
    // Disable canonical mode (line buffering) and echo
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    
    // Set standard input to non-blocking mode
    int oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
    
    // Try to read a character
    ch = getchar();
    
    // Restore original terminal settings immediately
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);
    
    if(ch == EOF) {
        ch = -1; // No key pressed
    }
    return ch;
}

// --- Constructor ---
TeleopCommander::TeleopCommander() : rclcpp::Node("teleop_commander") {
    // 1. Declare Parameters
    this->declare_parameter<double>("linear_speed", 1.0);
    this->declare_parameter<double>("angular_speed", 1.5);
    this->declare_parameter<std::string>("set_auto_mode_service", "/turtle_agent/set_auto_mode");

    linear_speed_ = this->get_parameter("linear_speed").as_double();
    angular_speed_ = this->get_parameter("angular_speed").as_double();
    set_auto_mode_service_ = this->get_parameter("set_auto_mode_service").as_string();

    // 2. Publishers
    pub_manual_cmd_vel_ = this->create_publisher<geometry_msgs::msg::Twist>("manual_cmd_vel", 10);

    // QoS Concept: "Transient Local" (Latched). 
    // Late joining subscribers will receive the last published message immediately.
    rclcpp::QoS latched_qos(rclcpp::KeepLast(1));
    latched_qos.transient_local();
    pub_teleop_active_ = this->create_publisher<std_msgs::msg::Bool>("teleop_active", latched_qos);

    // 3. Service Client
    // We create the client object. It will wait for the service to be available before calling.
    client_set_auto_mode_ = this->create_client<std_srvs::srv::SetBool>(set_auto_mode_service_);

    // 4. Save Terminal Settings
    // We must save the original settings so we can restore them in the destructor,
    // otherwise the user's terminal will be broken after the node shuts down!
    tcgetattr(STDIN_FILENO, &original_termios_);
    termios_modified_ = true;

    // Publish initial state (false = autonomous by default)
    publish_state();

    // 5. Timer to poll keyboard at 50Hz (20ms)
    timer_ = this->create_wall_timer(
        std::chrono::milliseconds(20),
        std::bind(&TeleopCommander::read_keyboard, this));

    RCLCPP_INFO(this->get_logger(), "TeleopCommander ready.");
    RCLCPP_INFO(this->get_logger(), "Controls: w/s=fwd/back, a/d=turn, q=override ON, e=override OFF");
}

// --- Destructor ---
TeleopCommander::~TeleopCommander() {
    if (termios_modified_) {
        // CRITICAL: Restore the user's terminal settings when the node is killed (Ctrl+C)
        tcsetattr(STDIN_FILENO, TCSANOW, &original_termios_);
    }
}

// --- Publish State ---
void TeleopCommander::publish_state() {
    std_msgs::msg::Bool state;
    state.data = override_active_;
    pub_teleop_active_->publish(state);
}

// --- Toggle Override ---
void TeleopCommander::set_override(bool active) {
    if (override_active_ == active) return; // No change, do nothing

    override_active_ = active;
    publish_state(); // Tell the world we changed state

    // Call the mux's service to toggle auto/manual mode
    if (client_set_auto_mode_->service_is_ready()) {
        auto request = std::make_shared<std_srvs::srv::SetBool::Request>();
        request->data = !active; // If active=true (manual), auto_mode=false on the mux
        
        // async_send_request is NON-BLOCKING. It sends the request and moves on.
        // We must NOT block inside a timer callback!
        client_set_auto_mode_->async_send_request(request);
        RCLCPP_INFO(this->get_logger(), "Override %s. (Mux auto_mode=%s)", 
                    active ? "ON" : "OFF", active ? "false" : "true");
    } else {
        RCLCPP_WARN(this->get_logger(), "Mux service not available! State changed locally only.");
    }

    if (!active) {
        stop_robot(); // Stop the robot when releasing override so it doesn't keep the last command
    }
}

void TeleopCommander::stop_robot() {
    geometry_msgs::msg::Twist stop;
    pub_manual_cmd_vel_->publish(stop);
}

// --- Keyboard Polling ---
void TeleopCommander::read_keyboard() {
    int ch = getch_nonblocking();
    if (ch < 0) return; // No key pressed

    geometry_msgs::msg::Twist cmd;

    switch (ch) {
        case 'q': set_override(true); return;
        case 'e': set_override(false); return;
        case 'w': cmd.linear.x = linear_speed_; break;
        case 's': cmd.linear.x = -linear_speed_; break;
        case 'a': cmd.angular.z = angular_speed_; break;
        case 'd': cmd.angular.z = -angular_speed_; break;
        case 'x': stop_robot(); return; // Explicit stop
        default: return; // Ignore unmapped keys
    }

    // Only publish movement if override is currently active
    if (override_active_) {
        pub_manual_cmd_vel_->publish(cmd);
    }
}

}  // namespace intro_teleop_commander