# import time
#
# import rospy
#
# from BaseNode import BaseNode
# from nav_msgs.msg import Odometry
# from scripts.v4.msg_parser.OdometryParser import OdometryParser
# import redis
#
#
#
# def callback_func(msg,cache,topic):
#     if not isinstance(msg,Odometry):
#         raise TypeError("Wrong message Types!")
#
#     parser = OdometryParser('Odometry')
#     msg = parser.parse(msg,topic)
#
#     for i in range(len(cache._cache)):
#         if cache._cache[i].is_full():
#             pass
#         else:
#             try:
#                 cache._cache[i].eneque(msg)
#             except Exception:
#                 print 'Cache is full or in Using! Will wait for a few moments'
#                 i +=1
#                 try:
#                     cache._cache[i].eneque(msg)
#                 except IndexError:
#                     print 'Out of the range!'
#                     time.sleep(1)
#                     cache.append()
#                     cache._cache[cache._num].eneque(msg)
#
# class OdometryNode(BaseNode):
#     # __slots__ = ('_topic','_types','_node_name')
#     def __init__(self,topic,nodename):
#         BaseNode.__init__(topic,nodename)
#         self._types = 'Odomtery'
#
#     def monitor(self,cache):
#         rospy.init_node(self._node_name)
#         rate = rospy.Rate(0.1)
#         while not rospy.is_shutdown():
#             rospy.Subscriber(self._topic,Odometry,callback=callback_func,callback_args=(cache,str(self._topic)),queue_size=10,buff_size=52428800)
#             rate.sleep()
#
#     def stop(self):
#         rospy.signal_shutdown('user oper.')