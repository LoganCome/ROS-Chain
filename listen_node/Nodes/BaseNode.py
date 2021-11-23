from abc import ABCMeta,abstractmethod
from scripts.v4.circle_queue import queue

class BaseNode(object,metaclass=ABCMeta):

    __slots__ = ('_topic','_types','_node_name')
    def __init__(self,topic,node_name):
        self._node_name = node_name
        self._topic = topic
        self._types = ''
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

    @abstractmethod
    def monitor(self,cache):
        pass
