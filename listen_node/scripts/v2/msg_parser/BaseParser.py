#! /usr/bin/env python
# coding: utf-8
import time
from abc import ABCMeta,abstractmethod
from nav_msgs.msg import Odometry
from sensor_msgs.msg import Image
from sensor_msgs.msg import Imu
from sensor_msgs.msg import CompressedImage

class BaseParser(object):
    """
    Base class of Parsers
    """
    __metaclass__ = ABCMeta
    __slots__ = ('_type','_enc_method')

    def __init__(self,type,enc_method='SM4'):
        self._type = type
        self._enc_method = enc_method


    def display(self):
        print ('type of the msg is of %s type' % (self._type))

    @property
    def msg_type(self):
        return self._type

    @msg_type.setter
    def msg_type(self,type):
        self._type = type

    '''Reserve for extension'''
    @staticmethod
    def GetType(msg):
        if isinstance(msg,Odometry):
            return 'Odometry'
        elif isinstance(msg,Image):
            return 'Image'
        elif isinstance(msg,CompressedImage):
            return 'CompressedImage'
        elif isinstance(msg,Imu):
            return 'Imu'
        else:
            pass

    @abstractmethod
    def parse(self,msg):
        pass
