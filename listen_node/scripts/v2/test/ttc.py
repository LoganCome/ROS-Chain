from socket import *
import json

ip = '120.27.224.138'
port = 8849
tcp_client = socket(AF_INET,SOCK_STREAM)

if __name__ == '__main__':
    tcp_client.connect((ip,port))
    msg = json.dumps({'dsds':11,'ddss':1})
    tcp_client.send(msg.encode('utf-8'))

    data = tcp_client.recv(1024).decode('utf-8')
    print(data)

    tcp_client.close()