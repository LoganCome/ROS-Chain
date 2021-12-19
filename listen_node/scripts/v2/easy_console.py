
import time
from File_oper import File
from socket import *
from consol.deploys import Connect_redis

from consol.BasePacker import BasePacker


topic_names = []
Types = []
User_name = None
IP = ''
keys = ''
UDP_Send_Port = ''
Udp_Recv_Port = ''
TCP_Port = ''

# lock = Manager().Lock()


"""
    IP need to be appointed
"""
ip = '120.27.224.138'
port = 8849
tcp_cli = socket(AF_INET,SOCK_STREAM)

# UDP_cli = UDP_Client('8.130.14.55',8849)

def Variable_Init():
    global topic_names
    global Types
    global keys
    global User_name
    global node_names

    topic_names = File('./register/deploy_args/topic_names.txt').read_txt()
    Types = File('./register/deploy_args/Types.txt').read_txt()
    keys = File('./register/deploy_args/keys.txt').read_txt()



if __name__ == '__main__':

    # TCP_ip,TCP_port = TCP_cli.target_ip_port_detection()
    # print TCP_cli.test()
    username = File('./register/deploy_args/Username.txt').read_txt()
    # passwd = File('./deploy_args/passwd.txt').read_txt()
    # res = BasePacker.command_send('verification',username=username,passwd=passwd,cli=TCP_cli)
    # if not res:
    #     exit(1)

    print ("Welcome to ROSWithChain!")
    time.sleep(1)

    topic_names = input('Please input the topics you want to monitor \n')
    if topic_names == '\n':
        time.sleep(1)
        pass
    else:
        topic = File('./register/deploy_args/topic_names.txt\n')
        topic.write_txt(topic_names)

    types_ = input('Please input the types of topics you just input\n')
    if types_ == '\n':
        time.sleep(1)
        pass
    else:
        types = File('./register/deploy_args/Types.txt')
        types.write_txt(types_)
        """
   commands: register / user_acc / verification / Monitor_start / Send / OnChain / Authority_Grant / Info_Check / Topics_reset / None
   """
    # Variable_Init()
    tcp_cli.connect((ip,port))
    while True:
        command = input("Please select the command to be performed(input the corresponding serial number)\n " \
                        "1.Send(Send the messages to the server) \n" \
                        "2.Info_Check(Check the messages on the Ethereum) \n" \
                        "3.exit\n")
        if command == '1':
            all = input('Do you want to send all the messages?(True or False)\n')
            r = Connect_redis()
            if all == True:
                BasePacker.command_send('Send',redis=r,all=True,cli=tcp_cli,username=username)
            else:
                type_ = input('Please input the type of messages you want to send\n')
                BasePacker.command_send('Send',redis=r,all=False,cli=tcp_cli,username=username,type=type_)
        elif command == '2':
            BasePacker.command_send('Info_Check',username=username,cli=tcp_cli)
        elif command == '2':
            break

    print ("Thanks for using. If you think ROS chain is good, please give us a star!")

