"""
Name：ROS-Ethereum-基于ROS系统的机器人可信任信息加密共享框架
Development & Maintain: 海南大学 链聚智联团队
Acknowledgement: Bernie Liu(HKST) & Xiulai Li(HNU)
Contact: freak01716@163.com
"""
'''
后台入口程序
'''
import time
from redis_flush import Recv_data
import threading
from socket import *
lines = []
recv_threads = []
from chain_operations import Ethereum_info_get

if __name__ == '__main__':
    TCP_Socket = socket(AF_INET, SOCK_STREAM)
    # 绑定端口
    TCP_Socket.bind(('172.20.189.90', 8849))
    TCP_Socket.settimeout(10000000)
    TCP_Socket.listen(201)
    token, accounts = Ethereum_info_get()
    sem = threading.Semaphore(10)
    lock = threading.Lock()
    for i in range(10):
        recv_threads.append(threading.Thread(target=Recv_data,args=(TCP_Socket,sem,lock,token,accounts,)))
    try:
        for i in range(10):
            recv_threads[i].start()
        for i in range(10):
            recv_threads[i].join()
    except RuntimeError:
        time.sleep(10)




