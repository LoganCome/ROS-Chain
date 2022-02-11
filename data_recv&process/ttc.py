import time
from chain_operations import MESG_Send
from sm4 import encrypt_ecb,decrypt_ecb
import pyargs
from socket import *
import json

# if __name__ == '__main__':
#
#     TCP_Socket = socket(AF_INET, SOCK_STREAM)
#     # 绑定端口
#     TCP_Socket.bind(('172.20.189.90', 8849))
#     TCP_Socket.settimeout(10000000)
#     TCP_Socket.listen(201)
#     client_sock, client_addr = TCP_Socket.accept()
#     while True:
#         Recv_Data = json.loads(client_sock.recv(1024 * 1024).decode('utf-8'))
#         print(Recv_Data)