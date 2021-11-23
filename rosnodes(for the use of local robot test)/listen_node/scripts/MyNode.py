#! /usr/bin/env python
# coding: utf-8

import rospy
import time
from geometry_msgs.msg import PoseWithCovarianceStamped
from nav_msgs.msg import Odometry
from socket import *

# def SenData(x,y):
#     tcp_client = socket(AF_INET,SOCK_STREAM)
#     ip='192.168.1.101'
#     port = 8848
#     tcp_client.connect((ip,port))
#     str_x = str(x)
#     str_y = str(y)
#     res = str_x+"+"+str_y
#     tcp_client.send(res)

class SendMSG(object):


    def __init__(self,ip,port):
        local_addr = ('192.168.1.109',9090)
        self.ip = ip
        self.port = port
        self.udp_socket = socket(AF_INET,SOCK_DGRAM)
        self.udp_socket.bind(local_addr)
        self.dest_addr = (ip,port)

    def send_msg(self,msg):
        send_data = msg
        self.udp_socket.sendto(send_data.encode('utf-8'),self.dest_addr)
        self.udp_socket.close()

# def topic_callback(msg):
#     if not isinstance(msg,PoseWithCovarianceStamped):
#         return
#     print msg.pose.pose.position.x
#     print msg.pose.pose.position.y
#     print msg.pose.pose.position.z
#     print ("-----------------")
#     # print msg.pose.pose.orientation.x
#     # print msg.pose.pose.orientation.y
#     # print msg.pose.pose.orientation.z
#     # print msg.pose.pose.orientation.w
#     print ("-----------------")




def topic_callback2(msg):
    ip='192.168.1.109'
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
        udp = SendMSG(ip=ip,port=port)
        udp.send_msg(res)

        print _x
        print _y
        # print res
        print ("+++++++++++++++++")

if __name__ == '__main__':
    node_name = "listening_node"
    rospy.init_node(node_name)


    # rate = rospy.Rate(100)
    # topic_name = "amcl_pose"
    # while not rospy.is_shutdown():
    #     rospy.Subscriber(topic_name,PoseWithCovarianceStamped,callback=topic_callback)

    topic_name2 = "odom"
    rate = rospy.Rate(0.1)
    while not rospy.is_shutdown():
        rospy.Subscriber(topic_name2,Odometry,callback=topic_callback2,queue_size=1,buff_size=52428800)
        rate.sleep()
    # time.sleep(1)

