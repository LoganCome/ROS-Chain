import time
from File_oper import File
from Client.UDP_Client import UDP_Client
from Client.TCP_Client import TCP_Client
from circle_queue import queue
from multiprocessing import Manager
from scripts.v4.cache import Cache

topic_names = []
Types = []
node_names = []
User_name = None
IP = ''
keys = ''
UDP_Send_Port = ''
Udp_Recv_Port = ''
TCP_Port = ''
cache = Cache(8)
lock = Manager().Lock()


"""
    IP need to be appointed
"""
TCP_cli = TCP_Client(None,None)
UDP_cli = UDP_Client(None,None)

def Variable_Init():
    global topic_names
    global Types
    global keys
    global User_name
    global node_names

    topic_names = File('./deploy_args/topic_names.txt').read_txt()
    Types = File('./deploy_args/Types.txt').read_txt()
    keys = File('./deploy_args/keys.txt').read_txt()
    User_name = File('./deploy_args/Username.txt')
    for i in range(topic_names.__len__()):
        node_names.append('ROS-Chain_MonitorNode'+str(i))

def Send_Init(method):
    if isinstance(method,str):
        print 'TypeError!'
        exit(1)
    elif method == 'TCP':
        global TCP_Port
        IP,TCP_Port = TCP_cli.target_ip_port_detection()
    elif method == 'UDP':
        global UDP_Send_Port
        global Udp_Recv_Port
        IP,UDP_Send_Port = UDP_cli.target_ip_port_detection()
        IP,Udp_Recv_Port = UDP_cli.target_ip_port_detection()
    else:
        pass



if __name__ == '__main__':
    print "Welcome to ROSWithChain!"
    time.sleep(1)

    flag = File.isNull('./deploy_args/Username.txt')
    if flag == True:
        pass
    else:
        print '-------------Warning---------------\n' \
              'The name you enter is important, please keep that for future using!.\n'
        User_name = raw_input('Please input your username')
        name = File('./deploy_args/Username.txt')
        name.write_txt(User_name)

    topic_names = raw_input('Please input the topics you want to monitor')
    if topic_names == '\n':
        time.sleep(1)
        pass
    else:
        topic = File('./deploy_args/topic_names.txt')
        topic.write_txt(topic_names)

    topic_names = raw_input('Please input the types of topics you just input')
    if topic_names == '\n':
        time.sleep(1)
        pass
    else:
        topic = File('./deploy_args/Types.txt')
        topic.write_txt(topic_names)
    # main()
