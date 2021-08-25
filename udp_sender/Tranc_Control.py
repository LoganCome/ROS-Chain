# -*- coding = utf-8 -*-
# @Time:2021-08-26 1:18
# @Author:Freak
# @File:Tranc.py
# @SoftWare:PyCharm
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
"""
基于以太坊的无人车只能调度模型
交易信息下行操作模块

本项目由海南大学机器人与人工智能协会区块链小组成员共同维护
"""

#本机IP，会动态变化
URI = '192.168.1.101'
#发送端绑定端口
port3 = 9093
#交易信息接受端口
port1 = 8850
#交易信息发送端口
port2 = 9999
#小车编号
Robot_enode = None
#交易编号
Tranc_enode = None
#交易横坐标
Tranc_x = 0.0
#交易纵坐标
Tranc_y = 0.0
#线程池子
threads_pool = []

#UDP发送端抽象类
class Udp_cli(object):
    # 初始化
    def __init__(self, ip, port):
        # 发送端ip端口
        local_addr = (URI, port3)
        # 发送目的地ip
        self.ip = ip
        # 发送目的地端口
        self.port = port
        # 发送协议
        self.udp_socket = socket(AF_INET, SOCK_DGRAM)
        # 发送端信息绑定
        self.udp_socket.bind(local_addr)
        # 发送地址
        self.dest_addr = (ip, port)
        # 发送函数
    ''':param
    msg:需要发送的信息'''
    def send_msg(self, msg):
        send_data = msg
        # 发送信息编码发送
        self.udp_socket.sendto(send_data.encode('utf-8'), self.dest_addr)
        # 关闭udp客户端
        self.udp_socket.close()

#随机标记生成
def Random_org():
    a = random.uniform(1629598000,1629598999)
    b = random.uniform(111111111,999999999)
    return a,b

#消息发送函数封装
def SEND(MSG):
    udp_cli = Udp_cli(ip=URI,port=port2)
    MSG = str(MSG)
    udp_cli.send_msg(MSG)


#subscriber回调函数
'''
@:param
msg:Odometry类型
enode:机器人编号
'''
def callback_action(msg,enode):
    #骗一下编译器
    if not isinstance(msg,Odometry):
        pass
    else:
        global Tranc_x
        global Tranc_y
        #获取即时的位置信息
        x = msg.pose.pose.position.x
        y = msg.pose.pose.position.y
        # 骗一下编译器
        if not isinstance(Tranc_x,float) and not isinstance(Tranc_y,float):
            pass
        else:
            #获取global变量
            Tranc_x = float(Tranc_x)
            Tranc_y = float(Tranc_y)
            #保留三位小数
            x = round(x,3)
            y = round(y,3)
            #交易执行完状态判定
            if abs(x-Tranc_x)<=0.1 and abs(y-Tranc_y)<=0.5:
                MSG = enode
                SEND(MSG)
                print ("Send Successfully")
            else:
                pass

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
            rospy.Subscriber("/robot_"+str(int(enode)-1)+"/odom",Odometry,callback=callback_action,callback_args=(enode,))

def MESG_Receive():
    Udp_Socket = socket(AF_INET, SOCK_DGRAM)
    Udp_Socket.bind(("192.168.1.101", port1))
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
        Tranc_x = Tranc_x/1000
        Tranc_y = Tranc_y/1000
        print (Robot_enode)
        print (Tranc_enode)
        print (Tranc_x)
        print (Tranc_y)
        i=0
        rate = rospy.Rate(0.3)
        while not rospy.is_shutdown():
            Tranc_Implement(Robot_enode,Tranc_enode,Tranc_x,Tranc_y)
            i += 1
            rate.sleep()
            if i == 2:
                break


if __name__ == '__main__':
    rospy.init_node("Tranc_Control")
    t1 = threading.Thread(target=MESG_Receive)
    t2 = threading.Thread(target=Trancisdone,args=(1,))
    t3 = threading.Thread(target=Trancisdone,args=(2,))
    t1.start()
    t2.start()
    t3.start()
    threads_pool.append(t1)
    threads_pool.append(t2)
    threads_pool.append(t3)
    for i in threads_pool:
        i.join()
    print("Tranc info parse complete")