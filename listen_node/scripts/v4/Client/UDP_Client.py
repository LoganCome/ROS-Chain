#! /usr/bin/env python
# coding: utf-8
import socket
from BaseSender import Client
import scripts.v4.pyargs
from socket import *
class UDP_Client(Client):

    def __init__(self,ip,port):
        Client.__init__(ip=ip,port=port)
        local_addr = (scripts.v4.pyargs.MASTER_IP, 9090)
        self._udp_socket = socket(AF_INET,SOCK_DGRAM)
        self._udp_socket.bind(local_addr)
        self.dest_addr = (self._ip,self._port)

    def send_msg(self,msg):
        send_data = msg
        self._udp_socket.sendto(send_data.encode('utf-8'),self.dest_addr)


    def target_ip_port_detection(self):
        for port in range(8848,9049):
            try:
                sender = UDP_Client(ip=self._ip,port=self._port)
            except socket.error:
                print('port:'+str(port)+'has been occupied! Now ready to switch to the next port!')
                continue
            print ("Now, your sending port has switched to "+str(port))
            self.dest_addr = (self._ip,port)
            return self._ip,self._port

    def close(self):
        self._udp_socket.close()