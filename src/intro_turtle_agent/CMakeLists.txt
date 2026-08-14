cmake_minimum_required(VERSION 3.8)
project(intro_turtle_agent)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(std_srvs REQUIRED)
find_package(intro_interfaces REQUIRED)

include_directories(include)                                                                                                                                                       
                                                                                                                                                                                        
     add_executable(turtle_agent src/turtle_agent.cpp)                                                                                                                                  
     ament_target_dependencies(turtle_agent rclcpp geometry_msgs std_srvs intro_interfaces)                                                                                             
                                                                                                                                                                                        
     add_executable(cmd_vel_mux src/cmd_vel_mux.cpp)                                                                                                                                    
     ament_target_dependencies(cmd_vel_mux rclcpp geometry_msgs std_srvs intro_interfaces)                                                                                              
                                                                                                                                                                                        
     install(TARGETS turtle_agent cmd_vel_mux DESTINATION lib/${PROJECT_NAME}) 

# Build the TurtleAgent executable
add_executable(turtle_agent
  src/turtle_agent.cpp
  src/main_turtle_agent.cpp
)
# Link ROS dependencies to the executable
ament_target_dependencies(turtle_agent rclcpp geometry_msgs)
# Tell CMake where to find header files
target_include_directories(turtle_agent PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

# Build the CmdVelMux executable
add_executable(cmd_vel_mux
  src/cmd_vel_mux.cpp
  src/main_cmd_vel_mux.cpp
)
ament_target_dependencies(cmd_vel_mux rclcpp geometry_msgs std_srvs)
target_include_directories(cmd_vel_mux PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

# Install the executables so 'ros2 run' can find them
install(TARGETS turtle_agent cmd_vel_mux
  DESTINATION lib/${PROJECT_NAME})


if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
