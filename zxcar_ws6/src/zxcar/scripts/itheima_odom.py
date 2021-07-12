#!/usr/bin/env python
#encoding:utf-8

import rospy
from geometry_msgs.msg import  Twist
from nav_msgs.msg import Odometry
import numpy as np
from tf.transformations import quaternion_from_euler

class ItheimaOdom():

    def __init__(self):
        rospy.init_node("zxcar_odom")

        self.x = 0;
        self.y = 0;

        self.theta = 0;

        self.lastUpdateTime = rospy.Time.now();

        self.linear_x = 0
        self.angular_z = 0;
        # 订阅外部线速度和角速度
        rospy.Subscriber("/zxcar/get_vel",Twist,self.vel_callback);
        # 对外发布里程计的数据
        self.odomPubliser = rospy.Publisher("/raw_odom",Odometry,queue_size=50);

        rospy.spin();

    def vel_callback(self,msg):
       current_time = rospy.Time.now();

       # 获取速度相关信息
       self.linear_x = msg.linear.x
       self.linear_y = msg.linear.y
       self.angular_z = msg.angular.z

       # 变化的事件差
       delta_time = current_time.to_sec() - self.lastUpdateTime.to_sec();
       self.lastUpdateTime = current_time;

       # 计算这段时间差内x,y方向的变化
       # delta_x = (self.linear_x * np.cos(self.theta) - self.linear_y*np.sin(self.theta))*delta_time
       # delta_y = (self.linear_x * np.sin(self.theta) + self.linear_y*np.cos(self.theta))*delta_time

       delta_x = (self.linear_x * np.cos(self.theta))*delta_time
       delta_y = (self.linear_x * np.sin(self.theta))*delta_time

       self.x += delta_x
       self.y += delta_y

       self.theta += self.angular_z * delta_time

       rotation = quaternion_from_euler(0,0,self.theta);

       odom = Odometry();
       odom.header.stamp = current_time;
       odom.header.frame_id = "odom"
       odom.child_frame_id = "base_footprint"

       odom.pose.pose.position.x = self.x
       odom.pose.pose.position.y = self.y
       odom.pose.pose.position.z = 0
       odom.pose.covariance[0] = 0.001;
       odom.pose.covariance[7] = 0.001;
       odom.pose.covariance[35] = 0.001;

       odom.pose.pose.orientation.x = rotation[0]
       odom.pose.pose.orientation.y = rotation[1]
       odom.pose.pose.orientation.z = rotation[2]
       odom.pose.pose.orientation.w = rotation[3]

       odom.twist.covariance[0] = 0.0001;
       odom.twist.covariance[7] = 0.0001;
       odom.twist.covariance[35] = 0.0001;
       odom.twist.twist.linear.x = self.linear_x
       odom.twist.twist.linear.y = self.linear_y

       odom.twist.twist.angular.z = self.angular_z

       self.odomPubliser.publish(odom);

if __name__ == '__main__':
    ItheimaOdom()