#! /usr/bin/env python
# coding: utf-8
import socket


import sys
import os
curPath = os.path.abspath(os.path.dirname('test.py'))
rootPath = os.path.split(curPath)[0]

sys.path.append(rootPath)


from Client.BaseSender import Client
from socket import *
class TCP_Client(Client):

    def __init__(self,ip,port):
        Client.__init__(self,ip=ip,port=port)
        self._tcp_socket = socket(AF_INET,SOCK_DGRAM)
        self._tcp_socket.settimeout(180)


    def connection_establish(self):
        self._tcp_socket.connect((self._ip,self._port))

    def target_ip_port_detection(self):
        for port in range(8849,9049):
            try:
                self.connection_establish()
            except socket.error:
                print('port:'+str(port)+'has been occupied! Now ready to switch to the next port!')
                continue
            print ("Now, your sending port has switched to "+str(port))
            self._port = port

            return self._ip,self._port

    def send_msg(self,msg):
        self.connection_establish()
        self._tcp_socket.send(msg.encode('utf-8'))

    def recv_msg(self):
        data = self._tcp_socket.recv(1024).decode('utf-8')
        return data

    def close(self):
        self._tcp_socket.close()

    def test(self):
        return self._port


