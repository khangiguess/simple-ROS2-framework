import sys
if sys.prefix == '/usr':
    sys.real_prefix = sys.prefix
    sys.prefix = sys.exec_prefix = '/home/khangiguess/uav/simple-ROS2-framework/install/intro_teleop_commander'
