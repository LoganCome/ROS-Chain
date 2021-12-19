#! /usr/bin/env python
# coding: utf-8
import time
from abc import ABCMeta,abstractmethod
from nav_msgs.msg import Odometry
from BaseParser import BaseParser


class OdometryParser(object):


    def __init__(self,type):
        self._type = type

    def parse(self,msg):
        if not isinstance(msg,Odometry):
            exit(1)
        stmp = time.time()
        date = time.localtime(stmp)
        format_time = time.strftime('%Y-%m-%d %H:%M:%S', date)
        recv_x = msg.pose.pose.position.x
        recv_y =  msg.pose.pose.position.y
        recv_vel_x = msg.twist.twist.linear.x
        recv_vel_y = msg.twist.twist.linear.y

        message_dict = dict(zip(['time','relation_x','relation_y','relation_linear_x','relation_linear_y','encryption_method','topic'],
                                [str(format_time),str(recv_x),str(recv_y ),str(recv_vel_x),str(recv_vel_y) ,'sm4','Odometry']))

        return message_dict