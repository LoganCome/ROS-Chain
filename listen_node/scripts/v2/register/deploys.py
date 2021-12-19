from File_oper import File
import multiprocessing
import json

import sys
import os
curPath = os.path.abspath(os.path.dirname('test.py'))
rootPath = os.path.split(curPath)[0]

sys.path.append(rootPath)
from SM.SM4 import encrypt_ecb
from File_oper import File
import redis


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
    while redis.llen('Odometry') > 0:
        message = redis.lpop('Odometry')
        message_time = message.get('time')
        message_relation_x = message.get('relation_x')
        message_relation_y = message.get('relation_y')
        message_relation_linear_x = message.get('relation_linear_x')
        message_relation_linear_y = message.get('relation_linear_y')
        message_topic = message.get('topic')

        message_user = user
        message_key = key
        pre_process_message = {'message_time':message_time,'message_relation_x':message_relation_x,'message_relation_y':message_relation_y,'message_relation_linear_x':message_relation_linear_x,
                                'message_relation_linear_y':message_relation_linear_y,'message_topic':message_topic,'type':'Odometry'}
        encryption_message = encrypt_ecb(str(pre_process_message),message_key)

        json_message = json.dumps({'message':encryption_message,'user':message_user,'command':None})
        cli.send(json_message.encode('utf-8'))

def Image_DeployToSend(redis,cli,user,key):
    pass
def Imu_DeployToSend(redis,cli,user,key):
    pass
def CompressedImage_DeployToSend(redis,cli,user,key):
    pass