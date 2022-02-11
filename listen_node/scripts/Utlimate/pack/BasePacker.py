'''
指令解析模块
'''
import json
import sys
import os
from pack.deploys import All_DeployToSend,Odomtery_DeployToSend,Image_DeployToSend,Imu_DeployToSend,CompressedImage_DeployToSend
from File_oper import File
from socket import *

curPath = os.path.abspath(os.path.dirname('BasePacker.py'))
rootPath = os.path.split(curPath)[0]
sys.path.append(rootPath)

# 指令解析实体类
class BasePacker(object):

    def __init__(self,key,port,username):
       self._key = key
       self._sendingport = port
       self._user = username

    """
    commands: register / verification / Monitor_start / Send / Authority_Grant / Info_Check
    """
    @staticmethod
    def command_send(command,**kwargs):
        # 注册指令
       if command == 'register':
           try:
               username = kwargs.get('username')
               passwd = kwargs.get('passwd')
               sm4_enc = kwargs.get('sm4_enc')
               cli = kwargs.get('cli')
               msg1 = json.dumps({'username':username,'passwd':passwd,'sm4':sm4_enc,'command':'register'})
               cli.send(msg1.encode('utf-8'))
               recv = cli.recv(1024).decode('utf-8')
               print (recv)
           except Exception:
               print ('Bad args! Please check!')
               exit(1)
       # 身份验证指令
       elif command == 'verification':
           try:
               username = kwargs.get('username')
               passwd = kwargs.get('passwd')
               cli = kwargs.get('cli')
               msg_sending = json.dumps({'username':username,'passwd':passwd,'command':'verification'})
               cli.send(msg_sending.encode('utf-8'))
               recv = cli.recv(1024).decode('utf-8')
               print (recv)
               if recv == 'Acc':
                   print ('identity verified.')
                   return True
               else:
                   print ('verification failure!')
                   return False
           except Exception:
               print ('Bad args! Please check!')
               exit(1)
       # 开启节点监听
       elif command == 'Monitor_start':
           try:
               os.popen('./monitor.sh')
           except Exception:
               print ('Script file execution failed!'\
                     'Please give corresponding file execution permission!')
               exit(1)
       # 发送消息
       elif command == 'Send':
           try:
               key = File('./deploy_args/keys.txt').read_txt()
               type_ = kwargs.get('type')
               if type_ == 'Odometry':
                   Odomtery_DeployToSend(kwargs.get('redis'),kwargs.get('username'),key)
               elif type_ == 'Image':
                   Image_DeployToSend(kwargs.get('redis'),kwargs.get('username'),key)
               elif type_ == 'Imu':
                   Imu_DeployToSend(kwargs.get('redis'),kwargs.get('username'),key)
               elif type_ == 'CompressedImage':
                   CompressedImage_DeployToSend(kwargs.get('redis'),kwargs.get('username'),key)
           except Exception as ex:
               print ('Exception:%s'%ex)
               exit(1)
       # 该接口已废弃
       elif command == 'OnChain':
           tcp_cli = kwargs.get('cli')
           json_message = json.dumps({'command':'OnChain'})
           tcp_cli.sned(json_message.encode('utf-8'))
       # 查询链上信息
       elif command == 'Info_Check':
           tcp_cli = socket(AF_INET,SOCK_STREAM)
           tcp_cli.connect(('120.27.224.138',8849))
           address_num = kwargs.get('address')
           username = kwargs.get('username')
           json_message = json.dumps({'command':'Info_Check','username':username,'address':address_num})
           tcp_cli.send(json_message.encode('utf-8'))
           recv = tcp_cli.recv(1024*1024).decode('utf-8')
           print (recv)
       # 查询链上信息（查询自己的信息）
       elif command == 'Info_Check_Own':
           tcp_cli = socket(AF_INET,SOCK_STREAM)
           tcp_cli.connect(('120.27.224.138',8849))
           username = kwargs.get('username')
           json_message = json.dumps({'command':'Info_Check_Own','username':username})
           tcp_cli.send(json_message.encode('utf-8'))
           recv = tcp_cli.recv(1024*1024).decode('utf-8')
           print (recv)
       # 权限授予
       elif command == 'Authority_Grant':
           tcp_cli = socket(AF_INET,SOCK_STREAM)
           tcp_cli.connect(('120.27.224.138',8849))
           address = kwargs.get('address_')
           username = kwargs.get('username')
           json_message = json.dumps({'command':'Authority_Grant','address':address,'username':username})
           tcp_cli.send(json_message.encode('utf-8'))
           recv = tcp_cli.recv(1024*1024).decode('utf-8')
           print (recv)
       # 测试接口 已废弃
       elif command == 'easy_register':
           # try:
            username = kwargs.get('username')
            # passwd = kwargs.get('passwd')
            sm4_enc = kwargs.get('sm4_enc')
            cli = kwargs.get('cli')
            msg1 = json.dumps({'username':username,'command':'easy_register','sm4_enc':sm4_enc})
            cli.send(msg1.encode('utf-8'))
            recv = cli.recv(1024).decode('utf-8')
            print ("Your username is "+recv)
            print ('Register successfully!')

