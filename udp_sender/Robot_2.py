#! /usr/bin/env python
# coding: utf-8

import rospy
import time
from geometry_msgs.msg import PoseWithCovarianceStamped
from nav_msgs.msg import Odometry
from socket import *


Master_URI = '192.168.1.107'


class Robot_1_SendMSG(object):

    def __init__(self,ip,port):
        local_addr = (Master_URI,9090)
        self.ip = ip
        self.port = port
        self.udp_socket = socket(AF_INET,SOCK_DGRAM)
        self.udp_socket.bind(local_addr)
        self.dest_addr = (ip,port)

    def send_msg(self,msg):
        send_data = msg
        self.udp_socket.sendto(send_data.encode('utf-8'),self.dest_addr)
        self.udp_socket.close()



def robot_1_topic_callback2(msg):
    # ip='192.168.1.107'
    port = 8848
    if not isinstance(msg,Odometry):
        return
    print ("-----------------")
    tmp_x = 0
    tmp_y = 0
    _x = msg.pose.pose.position.x
    _y =  msg.pose.pose.position.y

    _x = _x * 1000
    _y = _y * 1000

    _x = int(_x)
    _y = int(_y)
    if _x == 0 and _y == 0:
        pass
    elif tmp_x == _x and tmp_y == _y:
        pass
    else:
        tmp_x = _x
        tmp_y = _y
        str_x = str(_x)
        str_y = str(_y)
        res = str_x+"+"+str_y
        udp = Robot_1_SendMSG(ip=Master_URI,port=port)
        udp.send_msg(res)

        print _x
        print _y
        # print res
        print ("+++++++++++++++++")

class Robot_2_SendMSG(object):

    def __init__(self,ip,port):
        local_addr = (Master_URI,9091)
        self.ip = ip
        self.port = port
        self.udp_socket = socket(AF_INET,SOCK_DGRAM)
        self.udp_socket.bind(local_addr)
        self.dest_addr = (ip,port)

    def send_msg(self,msg):
        send_data = msg
        self.udp_socket.sendto(send_data.encode('utf-8'),self.dest_addr)
        self.udp_socket.close()



def robot_2_topic_callback2(msg):
    # ip='192.168.1.107'
    port = 8849
    if not isinstance(msg,Odometry):
        return
    print ("-----------------")
    tmp_x = 0
    tmp_y = 0
    _x = msg.pose.pose.position.x
    _y =  msg.pose.pose.position.y

    _x = _x * 1000
    _y = _y * 1000

    _x = int(_x)
    _y = int(_y)
    if _x == 0 and _y == 0:
        pass
    elif tmp_x == _x and tmp_y == _y:
        pass
    else:
        tmp_x = _x
        tmp_y = _y
        str_x = str(_x)
        str_y = str(_y)
        res = str_x+"+"+str_y
        udp = Robot_1_SendMSG(ip=Master_URI,port=port)
        udp.send_msg(res)

        print _x
        print _y
        # print res
        print ("+++++++++++++++++")

if __name__ == '__main__':
    # node_name1 = "robot_1_listening_node"
    node_name2 = "robot_2_listening_node"
    # rospy.init_node(node_name1)
    rospy.init_node(node_name2)
    robot_1_topic_name = "/robot_0/odom"
    robot_2_topic_name = "/robot_1/odom"
    rate = rospy.Rate(0.1)
    while not rospy.is_shutdown():
        # rospy.Subscriber(robot_1_topic_name,Odometry,callback=robot_1_topic_callback2,queue_size=1,buff_size=52428800)
        rospy.Subscriber(robot_2_topic_name,Odometry,callback=robot_2_topic_callback2,queue_size=1,buff_size=52428800)
        rate.sleep()


