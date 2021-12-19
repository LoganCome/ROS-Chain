from socket import *
from scripts.v2.SM.SM4 import encrypt_ecb
from scripts.v2.File_oper import File
import json
ip = '120.27.224.138'
port = 8849
tcp_cli = socket(AF_INET,SOCK_STREAM)


if __name__ == '__main__':
    tcp_cli.connect((ip,port))
    pre_process_message = {'message_time':'message_time','message_relation_x':'5.2','message_relation_y':'5.2','message_relation_linear_x':'5.2',
                           'message_relation_linear_y':'5.2','message_topic':'message_topic','type':'Odometry'}
    encryption_message = encrypt_ecb(str(pre_process_message),'zsh')
    json_message = json.dumps({'message':encryption_message,'user':'ZSH','command':None})
    tcp_cli.send(json_message.encode('utf-8'))
    # username = File('./Username.txt').read_txt()
    # print str(username[0])
    # msg = json.dumps({'command':'Info_Check','username':'ZSH'})
    #
    # tcp_cli.send(msg.encode('utf-8'))
    # recv = json.loads(tcp_cli.recv(1024*1024).decode('utf-8'))
    # print recv