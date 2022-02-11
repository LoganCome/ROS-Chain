'''
信息打包发送函数
'''
import multiprocessing
import json
import sys
import os
import redis
from socket import *
from SM.SM4 import encrypt_ecb
from File_oper import File

curPath = os.path.abspath(os.path.dirname('deploys.py'))
rootPath = os.path.split(curPath)[0]
sys.path.append(rootPath)

tcp_cli = socket(AF_INET,SOCK_STREAM)
ip = '120.27.224.138'
port = 8849


def Connect_redis():
    r = redis.Redis(host='127.0.0.1', port=6379)
    print('Connection established!')
    return  r


def All_DeployToSend(redis,cli,user,key):
    types = File('./deploy_args/Types.txt').read_txt()
    len_ = len(types)
    if len_%2 == 0:
        pass
    else:
        types.append('end')

    process = []
    for i in range(len_/2):
        for i in range(2):
            if types[i] != 'end':
                tar = str(types[i])+'DeployToSend'
                process[i] = multiprocessing.Process(target=tar,args=(redis,cli,user,key,))
            else:
                pass
        for i in range(len(process)):
            process[i].start()
        for i in range(len(process)):
            process[i].join()
        types.remove(types[0])
        types.remove(types[1])
        len_ -= 2

def Odomtery_DeployToSend(redis,cli,user,key):
    tcp_cli.connect((ip,port))
    message_user = str(user[0])
    message_key = str(key[0])
    if redis.llen('Odometry') > 0:
        message = eval(redis.lpop('Odometry'))
        message['type'] = 'Odometry'
        # message_time = message.get('time')
        # message_relation_x = message.get('relation_x')
        # message_relation_y = message.get('relation_y')
        # message_relation_linear_x = message.get('relation_linear_x')
        # message_relation_linear_y = message.get('relation_linear_y')
        # message_topic = message.get('topic')
        # pre_process_message = {'message_time':message_time,'message_relation_x':message_relation_x,'message_relation_y':message_relation_y,'message_relation_linear_x':message_relation_linear_x,
        #                         'message_relation_linear_y':message_relation_linear_y,'message_topic':message_topic,'type':'Odometry'}
        encryption_message = encrypt_ecb(str(message),message_key)
        print (encryption_message)
        json_message = json.dumps({'message':encryption_message,'user':message_user,'command':None})
        tcp_cli.send(json_message.encode('utf-8'))
        print ('succ')
    # end_singal_msg = json.dumps({'message':'end','user':message_user,'command':None})
    # tcp_cli.send(end_singal_msg.encode('utf-8'))
    # print 'succ'

def Image_DeployToSend(redis,cli,user,key):
    pass
def Imu_DeployToSend(redis,cli,user,key):
    pass
def CompressedImage_DeployToSend(redis,cli,user,key):
    pass