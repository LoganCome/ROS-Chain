#! /usr/bin/env python
# coding: utf-8
import multiprocessing
import time

import pyargs
from Send_Control import read_txt
from Robot import monitor_node_plain_text
import circle_queue
import threading
# -*- coding = utf-8 -*-
# @Time:2021-10-19 16:00
# @Author:Freak
# @File:listen.py
# @SoftWare:PyCharm

"""
ROSChain

文件读取+节点监听模块
读取相应配置参数内容存入pyargs.py内存储的相应变量内
（配置参数格式参照见文件夹deploy_args）
pyargs是全局变量管理器

本项目由海南大学机器人与人工智能协会区块链小组成员共同维护
"""
#读取+节点监听模块
def listen():
    #sm4密钥读取
    pyargs.fileread_threads[0] = threading.Thread(target=read_txt,args=('./deploy_args/sm4_key.txt',pyargs.lines,0,))
    #节点名称读取
    pyargs.fileread_threads[1] = threading.Thread(target=read_txt,args=('./deploy_args/node_names.txt',pyargs.lines,1,))
    #监听端口读取
    pyargs.fileread_threads[2] = threading.Thread(target=read_txt,args=('./deploy_args/ports.txt',pyargs.lines,2,))
    #topic话题名称读取
    pyargs.fileread_threads[3] = threading.Thread(target=read_txt,args=('./deploy_args/topic_names.txt',pyargs.lines,3,))
    #sm2公钥读取（当前版本尚未使用）
    pyargs.fileread_threads[4] = threading.Thread(target=read_txt,args=('./deploy_args/sm2_pubkey.txt',pyargs.lines,4,))

    #开启文件读取线程
    for i in range(5):
        pyargs.fileread_threads[i].start()
    for i in range(5):
        pyargs.fileread_threads[i].join()

    #为相应数量的机器人初始化消息接收队列
    for i in range(pyargs.lines[1].__len__()):
        # pyargs.sm4_queue.append(queue(10))
        pyargs.plain_queue.append(circle_queue.queue(10))
        print isinstance(pyargs.plain_queue[i],circle_queue.queue)

if __name__ == '__main__':

    listen()

    for i in range(pyargs.lines[1].__len__()):
        try:
            re = []
            re.append(pyargs.CRYPT_END_CRIPTMESSAGE_TARGET_IP)
            re.append(int(pyargs.lines[2][i]))
            re.append(pyargs.lines[0][i])
            re.append(int(i+1))
            monitor_node_plain_text(pyargs.lines[1][i],pyargs.lines[3][i],re)
        except KeyboardInterrupt:
            print "User killed the process! If u want to continue, please restart."
            break
        except Warning:
            print "ROS can only proceed in main process!"
            break
        except Exception:
            print "Connection may exist some problems, please check the ip or port."
            break
        else:
            continue
    # thread = []
    # for i in range(4):
    #     thread.append(multiprocessing.Process(target=monitor_node_plain_text,args=(pyargs.lines[1][0],pyargs.lines[3][0],re)))
    # for i in range(4):
    #     thread[i].start()



