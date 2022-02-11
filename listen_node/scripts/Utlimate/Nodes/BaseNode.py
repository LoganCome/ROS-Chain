'''
监听节点实体类
'''
import rospy
from callbacks import Connect_redis
from callbacks import Odometry_callback_func,Image_callback_func,Imu_callback_func,CompressedImage_callback_func

class BaseNode(object):

    def __init__(self,topics,node_names,types):
        self._node_name = node_names
        self._topic = topics
        self._types = types

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

    # 监听函数
    def monitor(self):
        # redis连接
        r = Connect_redis(self._types)
        # 节点初始化
        rospy.init_node(self._node_name)
        # 设置监听频率
        rate = rospy.Rate(0.1)
        while not rospy.is_shutdown():
            for i in range(len(self._topic)):
                # 回调函数方法名拼接
                call_name = str(self._types[i])+'_callback_func'
                # 开启监听
                rospy.Subscriber(self._topic[i],self._types[i],callback=call_name,callback_args=(r,self._types[i]),queue_size=10,buff_size=52428800)
                rate.sleep()
    # 停止监听
    def stop(self):
        rospy.signal_shutdown('user oper.')
