import rospy
from callbacks import Odometry_callback_func,Connect_redis
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
        r = Connect_redis(self._types)
        rospy.init_node(self._node_name)
        rate = rospy.Rate(0.1)
        while not rospy.is_shutdown():
            for i in range(len(self._topic)):
                call_name = str(self._types[i])+'_callback_func'
                rospy.Subscriber(self._topic[i],self._types[i],callback=call_name,callback_args=(r,self._topic[i]),queue_size=10,buff_size=52428800)
            # rospy.Subscriber(self._topic,Odometry,callback=Odom_callback_func,callback_args=(cache,str(self._topic)),queue_size=10,buff_size=52428800)
                rate.sleep()

    def stop(self):
        rospy.signal_shutdown('user oper.')
