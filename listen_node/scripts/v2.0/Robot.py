#! /usr/bin/env python
# coding: utf-8

from nav_msgs.msg import Odometry
from socket import *
from SM4 import encrypt_ecb
import time
import pyargs
import rospy
import json

class Robot_SendMSG(object):

    def __init__(self,ip,port):
        local_addr = (pyargs.MASTER_IP,9090)
        self.ip = ip
        self.port = port
        self.udp_socket = socket(AF_INET,SOCK_DGRAM)
        self.udp_socket.bind(local_addr)
        self.dest_addr = (ip,port)

    def send_msg(self,msg):
        send_data = msg
        self.udp_socket.sendto(send_data.encode('utf-8'),self.dest_addr)
        self.udp_socket.close()

def crypt_message_send(res,ip,port,method,key,num):
    send_key = None
    if method == 1:
        method = 'sm4'
        send_key = key
    elif method == 2:
        method = 'sm2'
        send_key = key
    else:
        method = 'plain'
        send_key = None
    stmp = time.time()
    date = time.localtime(stmp)
    format_time = time.strftime('%Y-%m-%d %H:%M:%S', date)
    json_message = {str(format_time):res,'method':method,'key':send_key,'robot_num':num}
    try:
        crypt_message = encrypt_ecb(str(json_message),key=key)
    except RuntimeError:
        print "Encrypt failure, please try again."

    json_message = json.dumps({'message':crypt_message,'robot_num':num})
    udp = Robot_SendMSG(ip=ip,port=port)
    udp.send_msg(json_message)
    time.sleep(0.5)
    print 'Send message successfully!'

def monitor_node_sm4crypt(node_name,topic_name,queue,key):
    rospy.init_node(node_name)
    rate = rospy.Rate(0.1)
    while not rospy.is_shutdown():
        rospy.Subscriber(topic_name,Odometry,callback=robot_sm4_topic_callback,callback_args=(queue,key,),queue_size=10,buff_size=52428800)
        rate.sleep()

def monitor_node_plain_text(node_name,topic_name,lines):
    rospy.init_node(node_name)
    rate = rospy.Rate(0.1)
    while not rospy.is_shutdown():
        rospy.Subscriber(topic_name,Odometry,callback=callback_func,callback_args=lines,queue_size=10,buff_size=52428800)
        rate.sleep()

def recv_data(msg):
    if not isinstance(msg,Odometry):
        return
    recv_x = msg.pose.pose.position.x
    recv_y =  msg.pose.pose.position.y
    recv_vel_x = msg.twist.twist.linear.x
    recv_vel_y = msg.twist.twist.linear.y
    recv_x = int(recv_x * 1000)
    recv_y = int(recv_y * 1000)

    return recv_x,recv_y,recv_vel_x,recv_vel_y

def callback_func(msg,lines):
    if not isinstance(msg,Odometry):
        return
    print ("-----------------")
    _x,_y,vel_x,vel_y = recv_data(msg)
    print _x,_y,vel_x,vel_y
    res = str(_x)+"+"+str(_y)+'+'+str(vel_x)+'+'+str(vel_y)
    crypt_message_send(res,lines[0],lines[1],1,lines[2],lines[3])



def robot_plain_topic_callback(msg,queue):
    # port = 8848
    if not isinstance(msg,Odometry):
        return
    print ("-----------------")

    _x,_y,vel_x,vel_y = recv_data(msg)
    print queue.show_queue()
    try:
        temp_x = str(str(queue.get_queue_latest()).split('+')[0])
        temp_y = str(str(queue.get_queue_latest()).split('+')[1])
    except IndexError:
        pass

    if _x == 0 and _y == 0:
        pass
    elif temp_x == str(_x) and temp_y == str(_y):
        pass
    else:
        str_x = str(_x)
        str_y = str(_y)
        res = str_x+"+"+str_y+'+'+str(vel_x)+'+'+str(vel_y)
        if queue.is_full():
            queue.dequeue()
            queue.enqueue(res)
        else:
            queue.enqueue(res)
        # crypt_message = encrypt_ecb(res,'roschainbot1')
        # udp = Robot_SendMSG(ip=Master_URI,port=port)
        # udp.send_msg(res)
        # print 'Send message successfully!'

def robot_sm4_topic_callback(msg,queue,key):
    # ip='192.168.1.107'
    # port = 8848
    if not isinstance(msg,Odometry):
        return
    print ("-----------------")

    tmp_x = 0
    tmp_y = 0
    _x,_y,vel_x,vel_y = recv_data(msg)

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
        crypt_message = encrypt_ecb(res,key)
        if queue.is_full():
            queue.dequeue()
            queue.enqueue(crypt_message)
        else:
            queue.enqueue(crypt_message)