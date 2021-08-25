# -*- coding = utf-8 -*-
# @Time:2021-08-19 16:50
# @Author:Freak
# @File:dxada.py
# @SoftWare:PyCharm
import time
from web3 import Web3
import json
from redis_flush import data_obtain
import threading
from socket import *
import random

"""
基于以太坊的无人车只能调度模型
小车信息操作管理模块
交易信息上行操作模块

本项目由海南大学机器人与人工智能协会区块链小组成员共同维护
"""

#本机IP，会动态变化
URI = '192.168.1.101'
#发送端绑定端口
port1 = 9092
#交易信息发送端口
port2 = 8850
#交易信息回流端口
port3 = 9999
#初次上链操作标记
first_load = True

#根据约定设置机器人名称
Robot_1_name = "Robot_1"
Robot_2_name = "Robot_2"
#定义字典，方便以太坊账号操作以及链码初始化
Robot = {Robot_1_name : 0,Robot_2_name : 1}
#线程池
threads = []

#UDP发送端抽象类
class MESG_Send(object):
    #初始化
    def __init__(self, ip, port):
        #发送端ip端口
        local_addr = (URI, port1)
        #发送目的地ip
        self.ip = ip
        #发送目的地端口
        self.port = port
        #发送协议
        self.udp_socket = socket(AF_INET, SOCK_DGRAM)
        #发送端信息绑定
        self.udp_socket.bind(local_addr)
        #发送地址
        self.dest_addr = (ip, port)

    #发送函数
    ''':param
    msg:需要发送的信息'''
    def send_msg(self, msg):
        send_data = msg
        #发送信息编码发送
        self.udp_socket.sendto(send_data.encode('utf-8'), self.dest_addr)
        #关闭udp客户端
        self.udp_socket.close()

#随机交易信息生成（范围可调动，随地图信息而变）
def Random_Tranc(num):
    #横坐标
    b = random.uniform(-1.000,1.000)
    #纵坐标
    a = random.uniform(-1.000,1.000)
    return a,b

#交易信息上链模块
'''
@:param
token:合约对象token
accounts:以太坊账户（共有十个，下标从0开始）
num:上链交易数量
rate:交易上链间隔
'''
def Tranc_onchain(token,accounts,num,rate):
    for i in range(int(num)):
        #生成随机交易
        a ,b = Random_Tranc(i+1)
        a = int(a*1000)
        b = int(b*1000)
        #交易信息上链存储
        token.functions.Tranc_stor(i+1,a,b).transact({'from': accounts[0]})
        #交易信息查验
        (x,y,enode) = token.functions.Get_Tranc().call({'from': accounts[0]})
        print(x,y,enode)
        #停顿
        time.sleep(float(rate))

#交易执行节点信息的确定
def Tranc_ongoing(token,accounts):
    '''@:param
    Robot_node:交易执行小车编号
    Tranc_enode:执行交易编号
    Tranc_x:交易载体横坐标
    Tranc_y:交易载体纵坐标'''
    Robot_enode,Tranc_enode,Tranc_x,Tranc_y = token.functions.decision().call({'from':accounts[0]})
    #信息封装
    msg = str(Robot_enode)+"+"+str(Tranc_enode)+"+"+str(Tranc_x)+"+"+str(Tranc_y)
    print("The Transaction info to be send is listed as follow:\n")
    #信息打印
    print("The robot's enode is "+str(Robot_enode)+",\n"
          +"The transaction enode is "+str(Tranc_enode)+",\n"
          +"The transaction's x is "+str(Tranc_x)+",\n"
          +"The transaction's y is "+str(Tranc_y)+".")
    #Udp服务端初始化
    udp = MESG_Send(ip=URI,port=port2)
    #信息发送
    udp.send_msg(msg)
    print("Transaction info send successfully!")

#交易处理完毕的后续处理
def Tranc_isdone(token,accounts):
    #Udp客户端初始化
    Udp_Socket = socket(AF_INET, SOCK_DGRAM)
    #绑定端口
    Udp_Socket.bind((URI, port3))
    # 接受端口发送的数据
    Recv_Data = Udp_Socket.recvfrom(1024)
    # 数据解码清洗
    Recv_msg = Recv_Data[0].decode('utf-8')
    Recv_msg = Recv_msg.split(",")
    data = Recv_msg[0].split("(")
    Robot_enode = int(data[1])
    if Robot_enode != -1:
        #将执行完交易的下车工装状态调整为False
        token.functions.Tranc_status(Robot_enode).transact({'from':accounts[Robot_enode]})
        print("Calibration Complete")

#以太坊合约信息初始化
def Ethereum_info_get():
    w3 = Web3(Web3.HTTPProvider('http://localhost:8545'))
    # 加载合约账户
    accounts = w3.eth.accounts
    # 加载合约
    token = load_via_artifact(w3, 'Robotchain')
    return token,accounts

#根据本地链码的二进制文件以及以太坊上的链码部署地址初始化合约
def load_via_artifact(w3, contract):
    #二进制文件路径
    fn_abi = './{0}.abi'.format(contract)
    #地址文件路径
    fn_addr = './{0}.addr'.format(contract)
    #读取二进制文件
    with open(fn_abi) as f:
        abi = json.load(f)
    #读取地址文件
    with open(fn_addr) as f:
        addr = f.read()
    #初始化合约
    return w3.eth.contract(address=addr, abi=abi)

#上链操作管理
def data_on_chain(token,accounts,name):
    #引用全局的标记变量
    global first_load
    #调用链上函数，在区块链上注册机器人
    token.functions.registerRobot(Robot[name]+1).transact({'from': accounts[Robot[name]+1]})
    print(token.functions.getenode().call({'from': accounts[Robot[name]+1]}))
    print(token.functions.Get_num().call({'from': accounts[0]}))
    #从初次上链小车信息
    while first_load:
        #获取数据
        #x,为小车即时横坐标，y为小车即时纵坐标
        x,y = data_obtain(name)
        x = int(x)
        y = int(y)
        #调用链上方法，设置机器人位置信息
        token.functions.setPosition(x,y).transact({'from': accounts[Robot[name]+1]})
        #标记记为False，确保该函数不会被二次执行
        first_load = False
        #获取链上机器人位置
        (tx, ty) = token.functions.getOwnPosition().call({'from': accounts[Robot[name] + 1]})
        print("da"+str(tx)+str(ty))
    #第二次或之后的信息上链
    while True:
        #获取即时的小车位置信息
        x,y = data_obtain(name)
        x = int(x)
        y = int(y)
        #获取链上记录的同一台小车的上一组位置
        (tx, ty) = token.functions.getOwnPosition().call({'from': accounts[Robot[name] + 1]})
        #进行比对，如果相同则表示小车未移动，该次获取到的即时位置信息将被丢弃
        if int(tx) == x and int(ty) == y:
            # print(tx,ty)
            pass
        #否则将即时位置信息上链
        else:
            token.functions.setPosition(x, y).transact({'from': accounts[Robot[name]+1]})

#主函数
if  __name__ == "__main__":
    #输入交易的数量
    print("Please enter the numbers of Tranc")
    num = input()
    #输入交易上链的频率
    print("Please enter the rate of sending to chain")
    rate = input()
    #区块链合约对象、账户获取
    token,accounts = Ethereum_info_get()
    #第一台车的位置信息上链线程
    t1 = threading.Thread(target=data_on_chain,args=(token,accounts,Robot_1_name,))
    # 第二台车的位置信息上链线程
    t2 = threading.Thread(target=data_on_chain,args=(token,accounts,Robot_2_name,))
    #交易信息上链线程
    t3 = threading.Thread(target=Tranc_onchain,args=(token,accounts,num, rate,))
    #交易执行线程
    t4 = threading.Thread(target=Tranc_ongoing,args=(token,accounts,))
    #交易执行完毕后续操作线程
    t5 = threading.Thread(target=Tranc_isdone,args=(token,accounts,))
    #启动所有线程
    t1.start()
    t2.start()
    t3.start()
    #确保在交易信息全部上链完毕后再执行交易
    time.sleep(1)
    t4.start()
    #确保有在执行中的交易
    time.sleep(2)
    t5.start()
    #所有线程加入线程池
    threads.append(t1)
    threads.append(t2)
    threads.append(t3)
    threads.append(t4)
    threads.append(t5)
    #等待所有线程执行完毕
    for t in threads:
        t.join()