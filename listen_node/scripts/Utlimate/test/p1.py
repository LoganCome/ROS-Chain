import os
import subprocess
import rospy
from nav_msgs.msg import Odometry
def robot_2_topic_callback2():
    pass

if __name__ == '__main__':
    while True:
        s = input('Please input a num')
        if s == 1:
            os.popen('./test.sh')
        else:
            break