#! /usr/bin/env python
# coding: utf-8

import rospy
import time
from move_base_msgs.msg import MoveBaseActionGoal
from move_base_msgs.msg import MoveBaseActionResult
from nav_msgs.msg import Odometry
from socket import *
import random
import time
import threading


Robot_1_goal_topic_name = '/robot_0/move_base/goal'
Robot_2_goal_topic_name = '/robot_1/move_base/goal'
Robot_1_pos_topic_name = '/robot_0/odom'
Robot_2_pos_topic_name = '/robot_1/odom'
result = '/robot_1/move_base/result'
URI = '192.168.1.122'
port = 9093
port1 = 8850
port2 = 8851
Robot_enode = None
Tranc_enode = None
Tranc_x = None
Tranc_y = None
threads_pool = []

class Udp_cli(object):
    def __init__(self, ip, port):
        local_addr = (URI, port)
        self.ip = ip
        self.port = port
        self.udp_socket = socket(AF_INET, SOCK_DGRAM)
        self.udp_socket.bind(local_addr)
        self.dest_addr = (ip, port)

    def send_msg(self, msg):
        send_data = msg
        self.udp_socket.sendto(send_data.encode('utf-8'), self.dest_addr)
        self.udp_socket.close()


def Random_org():
    a = random.uniform(1629598000,1629598999)
    b = random.uniform(111111111,999999999)
    return a,b

def callback_action(msg):
    udp_cli = Udp_cli(ip=URI,port=port2)
    if not isinstance(msg,Odometry):
        pass
    else:
        global Tranc_x
        global Tranc_y
        x = msg.pose.pose.position.x
        y = msg.pose.pose.position.y
        print x
        print y
        if not isinstance(Tranc_x,float) and not isinstance(Tranc_y,float):
            pass
        else:
            Tranc_x = float(Tranc_x)
            Tranc_y = float(Tranc_y)
            if abs(x-Tranc_x)<0.2 and abs(x-Tranc_x)<0.2:
                MSG = Robot_enode
                udp_cli.send_msg(MSG)
                print ("Send Successfully")
            else:
                MSG = -1
                udp_cli.send_msg(MSG)


def Tranc_Implement(num,enode,x,y):
    a,b = Random_org()
    MoveBase = MoveBaseActionGoal()
    MoveBase.goal.target_pose.header.frame_id = 'map'
    MoveBase.goal.target_pose.header.seq = enode
    MoveBase.goal.target_pose.header.stamp.nsecs = b
    MoveBase.goal.target_pose.header.stamp.secs = a
    MoveBase.goal.target_pose.pose.orientation.x = 0.0
    MoveBase.goal.target_pose.pose.orientation.y = 0.0
    MoveBase.goal.target_pose.pose.orientation.z = 0.5
    MoveBase.goal.target_pose.pose.orientation.w = 0.5
    MoveBase.goal.target_pose.pose.position.x = float(x)
    MoveBase.goal.target_pose.pose.position.y = float(y)
    MoveBase.goal.target_pose.pose.position.z = 0.0
    MoveBase.goal_id.id = ''
    MoveBase.goal_id.stamp.secs = 0
    MoveBase.goal_id.stamp.nsecs = 0
    MoveBase.header.frame_id = ''
    MoveBase.header.seq = enode
    MoveBase.header.stamp.secs = a
    MoveBase.header.stamp.nsecs = b
    pub = rospy.Publisher("/robot_"+str(num-1)+"/move_base/goal",MoveBaseActionGoal,queue_size=100)
    pub.publish(MoveBase)

def Trancisdone(enode):
    if not isinstance(enode,int):
        pass
    else:
        while True:
            rospy.Subscriber("/robot_"+str(int(enode)-1)+"/odom",Odometry,callback=callback_action)

def MESG_Receive():
    Udp_Socket = socket(AF_INET, SOCK_DGRAM)
    Udp_Socket.bind(("192.168.1.122", port1))
    while True:
        #接受端口发送的数据
        Recv_Data = Udp_Socket.recvfrom(1024)
        #数据解码
        Recv_msg = Recv_Data[0].decode('utf-8')
        #数据分割处理
        Recv_msg = Recv_msg.split("+")
        global Robot_enode
        global Tranc_enode
        global Tranc_x
        global Tranc_y
        Robot_enode = int(Recv_msg[0])
        Tranc_enode = int(Recv_msg[1])
        Tranc_x = float(Recv_msg[2])
        Tranc_y = float(Recv_msg[3])
        Tranc_x = float(Tranc_x/1000)
        Tranc_y = float(Tranc_y/1000)
        print (Robot_enode)
        print (Tranc_enode)
        rate = rospy.Rate(0.1)
        i=0
        while not rospy.is_shutdown():
            Tranc_Implement(2,1,1.0,1.0)
            i += 1
            rate.sleep()
            if i == 2:
                break


if __name__ == '__main__':
    rospy.init_node("Tranc_Control")
    # i=0
    # rate = rospy.Rate(0.1)
    # while not rospy.is_shutdown():
    #     Tranc_Implement(2,1,1.0,1.0)
    #     i += 1
    #     rate.sleep()
    #     if i == 2:
    #         break
    t2 = threading.Thread(target=Trancisdone,args=(1,))
    t2.start()
    threads_pool.append(t2)
    for i in threads_pool:
        i.join()
