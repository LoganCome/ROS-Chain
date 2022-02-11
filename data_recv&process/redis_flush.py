'''
后台总控程序
'''
import threading
import time
from web3 import Web3
import redis
from socket import *
import json
import multiprocessing
import pyargs
import asyncio
from chain_operations import data_on_chain_copy
from sm2 import SM2
from sm4 import decrypt_ecb
from chain_operations import data_on_chain_cop,register,Info_Check,Authority_Grant
#线程开关控制变量
thread_on = True
sm2_1 = 61567107928132125138381822413

# 按下ctrl+c的回调函数
def sigint_handler(signum, frame):
    print("The thread of receiving data will be killed!")
    # 将全局的线程开关控制变量设False，即关闭线程
    global thread_on
    thread_on = False
    print("The killing is going on.")

def read_txt(name,lines):
    with open(name,'r') as f:
        context = f.readlines()
    for line in context:
        line =line.strip('\n')
        lines.append(line)
    return lines

# Redis连接函数
def Connect_redis():
    pool = redis.ConnectionPool(host='172.20.189.90', port=6379, deploy_contract=True,max_connections=10)
    r = redis.Redis(connection_pool=pool)
    print('Connection established!')
    return r

# 数据接收函数
def Recv_data(sock:socket,sem:threading.Semaphore,lock,token,accounts):
    r = redis.Redis(host='localhost', port=6379, decode_responses=True)
    while True:
        sem.acquire()
        client_sock,client_addr = sock.accept()
        msg = client_sock.recv(1024*1024).decode('utf-8')
        if msg != None:
            Recv_Data = json.loads(msg)
            print(Recv_Data)
            command = Recv_Data['command']
            try:
                if command == 'easy_register':
                    pass
                    lock.acquire()
                    sm = SM2()
                    try:
                        keys = r.keys()
                        if Recv_Data['username'] in keys:
                            print(Recv_Data['username'])
                            username = str(Recv_Data['username'] + str(len(keys)))
                            sm4_key = sm.decrypt(Recv_Data['sm4_enc'], sm2_1)
                            print(sm4_key)
                            r.rpush(username, sm4_key)
                            len_ = len(r.keys())
                            passwd = (len_ % 10) + (len_ % 10) * 10 + (len_ % 10) * 100
                            r.rpush(username, len_)
                            r.rpush(username, passwd)
                            register(token, accounts, len_, str(passwd))
                            client_sock.send(username.encode('utf-8'))
                        else:
                            print(Recv_Data['username'])
                            sm4_key = sm.decrypt(Recv_Data['sm4_enc'], sm2_1)
                            print(sm4_key)
                            r.rpush(Recv_Data['username'], sm4_key)
                            len_ = len(r.keys())
                            passwd = (len_ % 10) + (len_ % 10) * 10 + (len_ % 10) * 100
                            r.rpush(Recv_Data['username'], len_)
                            r.rpush(Recv_Data['username'], passwd)
                            register(token, accounts, len_, str(passwd))
                            client_sock.send(Recv_Data['username'].encode('utf-8'))

                    finally:
                        lock.release()
                        client_sock.close()
                elif command == 'register':
                    lock.acquire()
                    sm = SM2()
                    i = 0
                    try:
                        keys = r.keys()
                        if Recv_Data['username'] in keys:
                            msg = 'Sorry, the username has been registered, please choose another one'
                            client_sock.send(msg.encode('utf-8'))
                        else:
                            username = Recv_Data['username']
                            sm4_key = sm.decrypt(Recv_Data['sm4'], sm2_1)
                            passwd = Recv_Data['passwd']
                            r.rpush(username, sm4_key)
                            len_ = len(r.keys())
                            r.rpush(username, len_)
                            r.rpush(username, passwd)
                            back = register(token, accounts, len_, sm4_key)
                            if back != False:
                                client_sock.send(str('Register successfully!, your account address is ' + str(
                                    back) + ' please keep it for information checking').encode('utf-8'))
                                client_sock.close()
                            else:
                                client_sock.send('Register failure. Please contact the developer'.encode('utf-8'))
                                client_sock.close()
                    finally:
                        lock.release()
                        client_sock.close()
                elif command == 'user_acc':
                    lock.acquire()
                    sm = SM2()
                    i = 0
                    try:
                        username = Recv_Data['username']
                        sm4_key = sm.decrypt(Recv_Data['sm4'], sm2_1)
                        passwd = Recv_Data['passwd']
                        r.rpush(username, sm4_key)
                        len_ = len(r.keys())
                        r.rpush(username, len_)
                        r.rpush(username, passwd)
                        back = register(token, accounts, len_, sm4_key)
                        if back != False:
                            client_sock.send(str('Register successfully!, your account number is ' + str(
                                back) + ' please keep it for information checking').encode('utf-8'))
                            client_sock.close()
                        else:
                            client_sock.send('Register failure. Please contact the developer'.encode('utf-8'))
                            client_sock.close()
                    finally:
                        lock.release()
                        client_sock.close()
                elif command == 'Authority_Grant':
                    address_num = int(Recv_Data['address'])
                    username = str(Recv_Data['username'][0])
                    print(address_num)
                    print(username)
                    print(r.lindex(username, 1))
                    num = int(r.lindex(username, 1))
                    # sm4_key = str(r.lindex(username, 0))
                    res = Authority_Grant(token, accounts, num, address_num)
                    if res == True:
                        client_sock.send('Grant successfully!'.encode('utf-8'))
                        client_sock.close()
                    else:
                        client_sock.send('Grant failure. Please contact the developer'.encode('utf-8'))
                        client_sock.close()
                elif command == 'verification':
                    username = Recv_Data['username']
                    passwd = Recv_Data['passwd']
                    passwd = passwd[0]
                    print(username[0])
                    print(passwd)
                    passwd_ = str(r.lindex(str(username[0]), 2))
                    print(passwd_)
                    if str(passwd_) == str(passwd):
                        client_sock.send('Acc'.encode('utf-8'))
                    else:
                        client_sock.send('Wrong passwd'.encode('utf-8'))
                    client_sock.close()
                elif command == None:
                    user = Recv_Data['user']
                    message_enc = Recv_Data['message']
                    print(user)
                    num = int(r.lindex(user, 1))
                    passwd = str(r.lindex(user, 2))
                    res = data_on_chain_cop(token, accounts, message_enc, num, passwd)
                    print(res)
                    if res == True:
                        client_sock.close()
                    else:
                        client_sock.send('Messages received failure. Please contact the developer'.encode('utf-8'))
                        client_sock.close()
                elif command == 'Info_Check':
                    username = str(Recv_Data['username'][0])
                    num = int(r.lindex(username, 1))
                    name = str(Recv_Data['address'])
                    address_num = int(r.lindex(name,1))
                    passwd = str(r.lindex(name, 0))
                    # len_ = r.brpop(username)
                    msg = Info_Check(token, accounts, num, passwd, address_num)
                    msg = json.dumps({'msg': str(Web3.toText(msg[0]))})
                    client_sock.send(msg.encode('utf-8'))
                    client_sock.close()
                elif command == 'Info_Check_Own':
                    username = str(Recv_Data['username'][0])
                    print(username)
                    num = int(r.lindex(username, 1))
                    passwd = str(r.lindex(username, 0))
                    msg = Info_Check(token, accounts, num, passwd, num)
                    msg = json.dumps({'msg': str(Web3.toText(msg[0]))})
                    client_sock.send(msg.encode('utf-8'))
                    client_sock.close()
            finally:
                sem.release()
        else:
            pass

def Recv_data_test(IP:str,port):
    # 创建socket
    TCP_Socket = socket(AF_INET, SOCK_STREAM)
    # 绑定端口
    TCP_Socket.bind((IP, port))
    # TCP_Socket.settimeout(50000)
    Recv_data_copy = None
    TCP_Socket.listen(201)
    # 线程执行任务
    while thread_on:
        # 接受端口发送的数据
        Recv_Data = TCP_Socket.recvfrom(1024)
        # 数据解码
        Recv_msg = Recv_Data[0].decode('utf-8')
        # 数据分割处理
        # Recv_msg = Recv_msg.split("+")

        Recv_msg = json.loads(Recv_msg)
        Robot_num = str(Recv_msg['robot_num'])
        # method = Recv_msg['method']
        # # key = Recv_msg['key']
        # Recv_msg = str(Recv_msg)
        message = str(Recv_msg['message'])

        print("Data Receive from Robot_1! Now ready to send to flush.")
        # 处理数据
        data_process_for_SM4orPlain(Robot_num,message)

#数据处理函数
def data_process_for_SM4orPlain(Robot_num,message,r:redis):
    #建立redis连接
    # r = redis.Redis(host=pyargs.REDIS_HOST, port=pyargs.redis_port, decode_responses=True)
    length = r.llen(str(Robot_num))
    if length < 3:
        r.rpush(Robot_num,message)
        print("insert successfully")
    elif length == 3:
        r.lpop(str(Robot_num))
        r.rpush(Robot_num,message)
        print("insert successfully")
    else:
        r.lpop(str(Robot_num))

#数据获取函数
async def data_get(num,queue:multiprocessing.Manager().Queue):
    r = redis.Redis(host=pyargs.REDIS_HOST, port=pyargs.redis_port, decode_responses=True)
    length = r.llen(str(num))
    if length < 3:
        await asyncio.sleep(1)
        current_length = r.llen(str(num))
        if current_length == length:
            message = r.brpop(str(num))
            queue.put(message)
            await asyncio.sleep(1)
        elif current_length == 3:
            while True:
                message = r.brpop(str(num))
                queue.put(message)
                await asyncio.sleep(1)
    elif length == 3:
        message = r.brpop(str(num))
        await asyncio.sleep(1)
        current_length = r.llen(str(num))
        if current_length == 2:
            queue.put(message)
            await asyncio.sleep(1)
        elif current_length == 3:
            while True:
                message = r.brpop(str(num))
                queue.put(message)
                await asyncio.sleep(1)

def data_get_copy(token,accounts,key,lock,r:redis):
    # r = redis.Redis(host=pyargs.REDIS_HOST, port=pyargs.redis_port, decode_responses=True)
    keys = r.keys()
    for i in range(len(keys)):
        length = r.llen(str(keys[i]))
        if length < 3:
            time.sleep(1)
            current_length = r.llen(str(keys[i]))
            if current_length == length:
                lock.acquire()
                message = list(r.brpop(str(keys[i])))[1]
                lock.release()
                data_on_chain_copy(token,accounts,str(message),key)
                time.sleep(1)
            elif current_length == 3:
                while True:
                    lock.acquire()
                    message = list(r.brpop(str(keys[i])))[1]
                    lock.release()
                    data_on_chain_copy(token,accounts,str(message),key)
                    time.sleep(1)
        elif length == 3:
            lock.acquire()
            message = list(r.brpop(str(keys[i])))[1]
            lock.release()
            time.sleep(1)
            current_length = r.llen(str(keys[i]))
            if current_length == 2:
                data_on_chain_copy(token,accounts,str(message),key)
                time.sleep(1)
            elif current_length == 3:
                while True:
                    lock.acquire()
                    message = list(r.brpop(str(keys[i])))[1]
                    lock.release()
                    data_on_chain_copy(token,accounts,str(message),key)
                    time.sleep(1)


