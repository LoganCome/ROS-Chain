import time
from File_oper import File
from scripts.v4.Nodes.BaseNode import BaseNode
from Client.TCP_Client import TCP_Client


topic_names = []
Types = []
node_names = 'ROS-Chain_MonitorNode'
keys = ''
User_name = None
passwd = ''

IP = ''
TCP_Port = ''
# UDP_Send_Port = ''
# Udp_Recv_Port = ''


# lock = Manager().Lock()


"""
    IP need to be appointed
"""
TCP_cli = TCP_Client(IP,8848)
# UDP_cli = UDP_Client(None,8848)

def Variable_Init():
    global topic_names
    global Types
    global keys
    global User_name
    global passwd

    topic_names = File('./deploy_args/topic_names.txt').read_txt()
    Types = File('./deploy_args/Types.txt').read_txt()
    keys = File('./deploy_args/keys.txt').read_txt()
    User_name = File('./deploy_args/Username.txt').read_txt()
    passwd = File('./deploy_args/passwd.txt').read_txt()


def Send_Init(method):
    if isinstance(method,str):
        print 'TypeError!'
        exit(1)
    elif method == 'TCP':
        global TCP_Port
        IP,TCP_Port = TCP_cli.target_ip_port_detection()
    # elif method == 'UDP':
    #     global UDP_Send_Port
    #     global Udp_Recv_Port
    #     IP,UDP_Send_Port = UDP_cli.target_ip_port_detection()
    #     IP,Udp_Recv_Port = UDP_cli.target_ip_port_detection()
    else:
        pass



if __name__ == '__main__':

    TCP_ip,TCP_port = TCP_cli.target_ip_port_detection()
    Variable_Init()
    node = BaseNode(topic_names,Types,node_names)
    print "Welcome to the ROS-Chain Monitor Terminal." \
          "The process of monitoring is going to start."\
          "If you want to stop, just close the terminal."
    node.monitor()



