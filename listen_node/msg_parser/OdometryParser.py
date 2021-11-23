import time
from abc import ABCMeta,abstractmethod
from nav_msgs.msg import Odometry
from BaseParser import BaseParser


class OdometryParser(BaseParser):

    __slots__ = ('_msg','_time_stamp','_type','_enc_method')

    def __init__(self,type):
        BaseParser.__init__(type)

    def parse(self,msg,topic):
        if not isinstance(msg,Odometry):
            exit(1)
        stmp = time.time()
        date = time.localtime(stmp)
        format_time = time.strftime('%Y-%m-%d %H:%M:%S', date)
        message_dict = dict(zip(['time','relation_x','relation_y','relation_linear_x','relation_linear_y','encryption_method','topic'],
                                [str(format_time),str(self._msg.pose.pose.position.x),str(self._msg.pose.pose.position.y),str(self._msg.twist.twist.linear.x),str(self._msg.twist.twist.linear.y),str(self._enc_method),str(topic)]))

        return message_dict