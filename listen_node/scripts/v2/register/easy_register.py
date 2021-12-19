import socket
import time

import sys
import os

curPath = os.path.abspath(os.path.dirname('../test/test.py'))
rootPath = os.path.split(curPath)[0]
sys.path.append(rootPath)

from File_oper import File
from register.BasePacker import BasePacker
from SM.sm2 import SM2
from socket import *

ip = '120.27.224.138'
port = 8849
tcp_cli = socket(AF_INET,SOCK_STREAM)
sm2 = SM2()

if __name__ == '__main__':
    print ("Welcome to ROS-Chain!")
    time.sleep(1)
    tcp_cli.connect((ip,8849))
    #(17306647372647258202065509670508781263666725676507001139152134818400236623399, 11453717086706785235539232688293639348927824880395433328659014060861434266807)
    #61567107928132125138381822413
    print ('-------------Warning---------------\n' \
            'The name you enter is important, please keep that for future using!.\n')
    User_name = input('Please input your username')
    sm4_key = input("For the first use, please input your sm4 key for encryption.")
    DB = (17306647372647258202065509670508781263666725676507001139152134818400236623399, 11453717086706785235539232688293639348927824880395433328659014060861434266807)
    sm4_enc = sm2.encrypt(str(sm4_key),DB)
    while BasePacker.command_send('easy_register',cli=tcp_cli,username=User_name,sm4_enc=sm4_enc):
        User_name = input('Please choose your username again')
    name = File('./deploy_args/Username.txt')

    key_file = File('./deploy_args/keys.txt')
    name.write_txt(User_name)

    key_file.write_txt(sm4_key)

    tcp_cli.close()



