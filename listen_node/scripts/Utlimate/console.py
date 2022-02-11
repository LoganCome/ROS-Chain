#! /usr/bin/env python
'''
总控模块
'''
# -*- 需要运行在py2的环境中 -*-
import time
from socket import *
from File_oper import File
from pack.deploys import Connect_redis
from pack.BasePacker import BasePacker

#定义必要变量
topic_names = []
Types = []
User_name = None
IP = ''
keys = ''
UDP_Send_Port = ''
Udp_Recv_Port = ''
TCP_Port = ''
#TCP/IP实例
tcp_cli = socket(AF_INET,SOCK_STREAM)
#目标IP/Port
ip = '120.27.224.138'
port = 8849

#变量初始化
def Variable_Init():
    global topic_names
    global Types
    global keys

    topic_names = File('./deploy_args/topic_names.txt').read_txt()
    Types = File('./deploy_args/Types.txt').read_txt()
    keys = File('./deploy_args/keys.txt').read_txt()

if __name__ == '__main__':
    #连接服务器
    try:
        tcp_cli.connect((ip,port))
    except Exception:
        print ('Time out or unknow IP. Please Contact the Administrator.')
        exit(1)

    #读取用户名和密码
    username = File('./deploy_args/Username.txt').read_txt()
    passwd = File('./deploy_args/passwd.txt').read_txt()
    #身份验证
    res = BasePacker.command_send('verification',username=username,passwd=passwd,cli=tcp_cli)
    if not res:
        exit(1)

    print "Welcome to ROS-Ethereum!"
    time.sleep(1)
    #输入topic名称
    topic_names = raw_input('Please input the topics you want to monitor.(Please split every topic with space) \n')
    if topic_names == '\n':
        time.sleep(1)
        pass
    else:
        topic = File('./deploy_args/topic_names.txt')
        topic.write_txt(topic_names)
    #输入每个topic对应的类型
    types_ = raw_input('Please input the types of topics you just input.(Please split every type with space) \n')
    if types_ == '\n':
        time.sleep(1)
        pass
    else:
        types = File('./deploy_args/Types.txt')
        types.write_txt(types_)
    """
    总控模块具备以下指令集：
    commands: Monitor_start / Send / Authority_Grant / Info_Check / Topics_reset / Exit
    分别对应以下功能：
    1.开启节点监听
    2.节点消息发送
    3.权限赋予
    4.查询链上信息
    5.目标节点重置
    6.退出
    """
    while True:
        command = input("Please select the command to be performed(input the corresponding serial number)\n "\
                        "1.Monitor_start(Monitor the pre-set topics) \n"\
                        "2.Send(Send the messages to the server) \n" \
                        "3.Authority_Grant(grant the authority to other ROS-Chain users) \n"\
                        "4.Info_Check(Check the messages on the Ethereum) \n" \
                        "5.Topics_reset(reset the monitored topics) \n" \
                        "6.exit \n")
        if command == 1:
            BasePacker.command_send('Monitor_start')
        elif command == 2:
            redis = Connect_redis()
            type_ = raw_input('Please input the type of messages you want to send')
            BasePacker.command_send('Send',redis=redis,username=username,type=type_)
        elif command == 3:
            address = raw_input('Please enter the num of the address of account you want to grant authority')
            BasePacker.command_send('Authority_Grant',cli=tcp_cli,address_=address,username=username)
        elif command == 4:
            choice = raw_input('Whtether you want to check your own information? (True or False)')
            if(choice == 'True'):
                BasePacker.command_send('Info_Check_Own',username=username)
            else:
                address_num = raw_input('Please enter the name')
                BasePacker.command_send('Info_Check',address=address_num,username=username)
        elif command == 5:
            topic_names = raw_input('Please input the topics you want to monitor.(Please split every topic with space) \n')
            if topic_names == '\n':
                time.sleep(1)
                pass
            else:
                topic = File('./deploy_args/topic_names.txt')
                topic.write_txt(topic_names)
        elif command == 6:
            break
    print "Thanks for using. If you think ROS-Ethereum is good, please give us a star! \"" \
          "Our github address is: https://github.com/RobAI-Lab/ROS-Chain. Thanks for your support!"
