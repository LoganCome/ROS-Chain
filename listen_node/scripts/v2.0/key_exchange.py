#! /usr/bin/env python
# coding: utf-8
import json

from sm2 import SM2
from socket import *
from Robot import Robot_SendMSG
import pyargs

file_path = 'deploy_args/sm2_pubkey.txt'
file_path2 = 'deploy_args/sm4_key.txt'

def Recv_data(IP,port):
    # 创建socket
    Udp_Socket = socket(AF_INET, SOCK_DGRAM)
    # 绑定端口
    Udp_Socket.bind((IP, port))
    Recv_Data = Udp_Socket.recvfrom(1024)
    # 数据解码
    Recv_msg = Recv_Data[0].decode('utf-8')
    msg = json.loads(Recv_msg)
    return tuple(msg['sm2']),str(msg['crypt'])

def write2txt(name,txt):
    with open(name,'w') as file:
        file.write(txt+'\n')

if __name__ == '__main__':
    sm2 = SM2()
    public_key,private_key = sm2.key_produce()
    try:
        send = Robot_SendMSG(ip=pyargs.CRYPT_END_SEND_TARGET_IP,port=pyargs.CRYPT_END_SM2andSM4_SEND_PORT)
        public_key = json.dumps({'key':public_key})
        send.send_msg(public_key)
    except Exception:
        print "The targer IP isn't founded or the port is on using, please check!"
    else:
        print "the sm2 public key is being sending...."

    print "Send successfully!"

    try:
        sm2_public_key,sm4_crypt = Recv_data(pyargs.CRYPT_END_SEND_TARGET_IP,port=pyargs.CRYPT_END_RECV_PORT)

        write2txt(file_path,str(sm2_public_key))
        sm4_key = sm2.decrypt(sm4_crypt,private_key)

        write2txt(file_path2,sm4_key)
    except IOError:
        print "Fail to write to file, please check file path."
    except Exception:
        print "Receive exception! please check the ip or port."

