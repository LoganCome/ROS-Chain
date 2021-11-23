#! /usr/bin/env python
# coding: utf-8
import socket
# import scripts.v4.pyargs
from socket import *
from abc import ABCMeta,abstractmethod

class Client(object,metaclass=ABCMeta):

    __slots__ = ('_ip','_port')
    def __init__(self,ip,port):
        # local_addr = (scripts.v4.pyargs.MASTER_IP, 9090)
        self._ip = ip
        self._port = port
        # self.udp_socket.bind(local_addr)
        # self.dest_addr = (ip,port)

    @abstractmethod
    def send_msg(self,msg):
        pass

    @abstractmethod
    def connection_establish(self):
        pass

    @abstractmethod
    def target_ip_port_detection(self):
        pass

    @property
    def ip(self):
        return self._ip

    @ip.setter
    def ip(self,ip):
        self._ip = ip

    @property
    def port(self):
        return self._port

    @port.setter
    def port(self,port):
        self._port = port