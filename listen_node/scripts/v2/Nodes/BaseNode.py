#! /usr/bin/env python
# coding: utf-8
import rospy
import redis
from callbacks import Odometry_callback_func
from nav_msgs.msg import Odometry
class BaseNode(object):

    # __slots__ = ('_topic','_types','_node_name')
    def __init__(self,topics,node_names,types):
        self._node_name = node_names
        self._topic = topics
        self._types = types
        # for i in range(4):
        #     self._cache.append(queue())


    @property
    def topic(self):
        return self._topic

    @topic.setter
    def topic(self,topic):
        self._topic = topic

    @property
    def types(self):
        return self._types

    @types.setter
    def types(self,types):
        self._types = types

    @property
    def node_name(self):
        return self._node_name

    @node_name.setter
    def node_name(self,node_name):
        self._node_name = node_name

    # @abstractmethod
    # def pack_send_UDP(self):
    #     pass
    #
    # @abstractmethod
    # def pack_send_TCP(self):
    #     pass

    def monitor(self):

        print self._topic[0]
        print self._types[0]
        rospy.init_node(self._node_name)
        rate = rospy.Rate(0.1)
        while not rospy.is_shutdown():
            for i in range(len(self._topic)):
                if self._types[i] == 'Odometry':
                    rospy.Subscriber(str(self._topic[i]),Odometry,callback=Odometry_callback_func,callback_args=(self._types[i],),queue_size=10,buff_size=52428800)
            # rospy.Subscriber(self._topic,Odometry,callback=Odom_callback_func,callback_args=(cache,str(self._topic)),queue_size=10,buff_size=52428800)
                    rate.sleep()
    # def monitor(self):
    #
    #     print self._topic[0]
    #     print self._types[0]
    #     rospy.init_node(self._node_name)
    #     rate = rospy.Rate(0.1)
    #     while not rospy.is_shutdown():
    #         rospy.Subscriber(str(self._topic[0]),Odometry,callback=Odometry_callback_func,callback_args=(self._types[0],),queue_size=10,buff_size=52428800)
    #             # rospy.Subscriber(self._topic,Odometry,callback=Odom_callback_func,callback_args=(cache,str(self._topic)),queue_size=10,buff_size=52428800)
    #         rate.sleep()

    def stop(self):
        rospy.signal_shutdown('user oper.')
