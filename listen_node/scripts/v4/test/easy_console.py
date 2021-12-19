import time
from scripts.v4.File_oper import File
from scripts.v4.Client.UDP_Client import UDP_Client
from scripts.v4.Client.TCP_Client import TCP_Client
from scripts.v4.pack.deploys import Connect_redis
from scripts.v4.cache import Cache
from scripts.v4.pack.BasePacker import BasePacker


topic_names = []
Types = []
User_name = None
IP = ''
keys = ''
UDP_Send_Port = ''
Udp_Recv_Port = ''
TCP_Port = ''
cache = Cache(8)
# lock = Manager().Lock()


"""
    IP need to be appointed
"""
TCP_cli = TCP_Client(ip='8.130.14.55',port=8849)
# UDP_cli = UDP_Client('8.130.14.55',8849)

def Variable_Init():
    global topic_names
    global Types
    global keys
    global User_name
    global node_names

    topic_names = File('./deploy_args/topic_names.txt').read_txt()
    Types = File('./deploy_args/Types.txt').read_txt()
    keys = File('./deploy_args/keys.txt').read_txt()



if __name__ == '__main__':

    TCP_ip,TCP_port = TCP_cli.target_ip_port_detection()
    # print TCP_cli.test()
    username = File('./deploy_args/Username.txt').read_txt()
    # passwd = File('./deploy_args/passwd.txt').read_txt()
    # res = BasePacker.command_send('verification',username=username,passwd=passwd,cli=TCP_cli)
    # if not res:
    #     exit(1)

    print "Welcome to ROSWithChain!"
    time.sleep(1)

    topic_names = raw_input('Please input the topics you want to monitor')
    if topic_names == '\n':
        time.sleep(1)
        pass
    else:
        topic = File('./deploy_args/topic_names.txt')
        topic.write_txt(topic_names)

    types_ = raw_input('Please input the types of topics you just input')
    if types_ == '\n':
        time.sleep(1)
        pass
    else:
        types = File('./deploy_args/Types.txt')
        types.write_txt(types_)
        """
   commands: register / user_acc / verification / Monitor_start / Send / OnChain / Authority_Grant / Info_Check / Topics_reset / None
   """
    while True:
        command = input("Please select the command to be performed(input the corresponding serial number)\n " \
                        "1.Monitor_start(Monitor the pre-set topics) \n" \
                        "2.Send(Send the messages to the server) \n" \
                        "3.Info_Check(Check the messages on the Ethereum) \n" \
                        "4.exit")
        if command == 1:
            BasePacker.command_send('Monitor_start')
        elif command == 2:
            all = input('Do you want to send all the messages?(True or False)')
            redis = Connect_redis()
            if all == True:
                BasePacker.command_send('Send',redis=redis,all=True,cli=TCP_cli,username=username)
            else:
                type_ = input('Please input the type of messages you want to send')
                BasePacker.command_send('Send',redis=redis,all=False,cli=TCP_cli,username=username,type=type_)
        elif command == 3:
            pass
        elif command == 4:
            break

    print "Thanks for using. If you think ROS chain is good, please give us a star!"

