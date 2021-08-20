# -*- coding = utf-8 -*-
# @Time:2021-08-19 16:50
# @Author:Freak
# @File:dxada.py
# @SoftWare:PyCharm
import time
from web3 import Web3
import json
from redis_flush import data_obtain

"""
基于以太坊的无人车只能调度模型
上链操作管理模块

本项目由海南大学机器人与人工智能协会区块链小组成员共同维护
"""

#根据约定设置机器人名称
Robot_1_name = "Robot_1"
Robot_2_name = "Robot_2"
#定义字典，方便以太坊账号操作以及链码初始化
Robot = {Robot_1_name : 0,Robot_2_name : 1}

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
def data_on_chain(name):
    #初始化web3对象
    w3 = Web3(Web3.HTTPProvider('http://localhost:8545'))
    #加载合约账户
    accounts = w3.eth.accounts
    #加载合约
    token = load_via_artifact(w3, 'Robotchain')
    #调用链上函数，在区块链上注册机器人
    token.functions.registerRobot(Robot[name]+1).transact({'from': accounts[Robot[name]]})
    #从redis中获取数据
    while True:
        #获取数据
        x,y = data_obtain(name)
        x = int(x)
        y = int(y)
        #调用链上方法，设置机器人位置信息
        token.functions.setPosition(x,y).transact({'from': accounts[Robot[name]]})
        #获取链上机器人位置
        (tx, ty) = token.functions.getOwnPosition().call({'from': accounts[0]})
        cur_time = time.time()
        #打印
        print("Current_Time: "+str(cur_time)+", and the cur position is :"+str(tx)+", "+str(ty))

if  __name__ == "__main__":
    data_on_chain(Robot_1_name)
    data_on_chain(Robot_2_name)