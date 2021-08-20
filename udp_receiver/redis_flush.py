# -*- coding = utf-8 -*-
# @Time:2021-08-19 16:14
# @Author:Freak
# @File:flush.py
# @SoftWare:PyCharm
"""
基于以太坊的无人车只能调度模型
Redis缓存

说明：由于ROS pub信息的速率过高，而强制降低消息发送速率会影响底盘信息调制。
从而影响建图、导航等相关功能
因此做一层缓存，将数据存储到redis中去，并且手动进行去重存储
这样不仅能规避消息发送速率过高的负面影响，而且可以保证区块上记录的信息为最新的信息，有效降低区块链负载

本项目由海南大学机器人与人工智能协会区块链小组成员共同维护
"""
import redis
from socket import *
import threading
import signal

#线程池子，初始化为空
threads = []
#线程开关控制变量
thread_on = True
#本机IP，用作Redis连接以及端口监听
host = 'localhost'
#Redis连接端口号
port = 6379
#机器人1信息监听端口
Robot_1_port = 8848
#机器人2信息监听端口
Robot_2_port = 8849
#机器人1名字
Robot_1_name = "Robot_1"
#机器人2名字
Robot_2_name = "Robot_2"

#按下ctrl+c的回调函数
def sigint_handler(signum, frame):
    print("The thread of receiving data will be killed!")
    #将全局的线程开关控制变量设False，即关闭线程
    global thread_on
    thread_on = False
    print("The killing is going on.")

#Redis连接函数
def Connect_redis():
    pool = redis.ConnectionPool(host=host, port=port, deploy_contract=True)
    r = redis.Redis(host=host, port=port, decode_responses=True)
    # list初始化
    r.rpush(Robot_1_name + "_x", 0)
    r.rpush(Robot_1_name + "_y", 0)
    r.rpush(Robot_2_name + "_x", 0)
    r.rpush(Robot_2_name + "_y", 0)
    print('Connection established!')

#机器人1端口监听
def Recv_data_from_Robot_1():
    #创建socket
    Udp_Socket = socket(AF_INET, SOCK_DGRAM)
    #绑定端口
    Udp_Socket.bind(("192.168.1.118", Robot_1_port))
    #线程执行任务
    while thread_on:
        #接受端口发送的数据
        Recv_Data = Udp_Socket.recvfrom(1024)
        #数据解码
        Recv_msg = Recv_Data[0].decode('utf-8')
        #数据分割处理
        Recv_msg = Recv_msg.split("+")

        tar_x = int(Recv_msg[0])
        tar_y = int(Recv_msg[1])
        print("Data Receive from Robot_1! Now ready to process.")
        #处理数据
        data_process(tar_x, tar_y, Robot_1_name)

#机器人2端口监听
def Recv_data_from_Robot_2():
    Udp_Socket = socket(AF_INET, SOCK_DGRAM)
    Udp_Socket.bind(("192.168.1.118", Robot_2_port))
    while 1:
        Recv_Data = Udp_Socket.recvfrom(1024)
        Recv_msg = Recv_Data[0].decode('utf-8')
        Recv_msg = Recv_msg.split("+")

        tar_x = int(Recv_msg[0])
        tar_y = int(Recv_msg[1])
        print("Data Receive from Robot_2! Now ready to process.")
        data_process(tar_x, tar_y, Robot_2_name)

#数据处理函数
def data_process(x, y, name):
    #建立redis连接
    r = redis.Redis(host=host, port=port, decode_responses=True)

    temp_x = int(r.brpop(name + "_x")[1])
    temp_y = int(r.brpop(name + "_y")[1])
    #比较此时拿到的数据和list中上一个数据新型比对，若相同则不入库，否则append到末尾
    if x == temp_x and y == temp_y:
        r.rpush(name + "_x", temp_x)
        r.rpush(name + "_y", temp_y)
    else:
        r.rpush(name + "_x", temp_x)
        r.rpush(name + "_y", temp_y)
        r.rpush(name + "_x", x)
        r.rpush(name + "_y", y)
        print("insert successfully")

#数据获取函数
def data_obtain(name):
    #建立连接
    r = redis.Redis(host=host, port=port, decode_responses=True)
    #获取List的长度，以get到list中最后一个信息
    length = r.llen(name + "_x")
    #获取list中最新的信息
    x = r.lindex(name + "_x", length - 1)
    y = r.lindex(name + "_y", length - 1)
    r.brpop(name + "_x")
    r.brpop(name + "_y")

    return x,y

#主函数
if __name__ == '__main__':
    #Redis建立连接
    Connect_redis()

    #创建两条线程，分别用于监听两台车的数据
    t1 = threading.Thread(target=Recv_data_from_Robot_1)
    t2 = threading.Thread(target=Recv_data_from_Robot_2)
    '''开启线程'''
    t1.start()
    t2.start()
    #线程加入线程池
    threads.append(t1)
    threads.append(t2)
    #信号接收
    signal.signal(signal.SIGINT, sigint_handler)
    #阻塞子线程线程
    for t in threads:
        t.join()
    #子线程执行完毕，回到主线程执行一下语句后，程序执行完毕
    print("Flush closed!")

