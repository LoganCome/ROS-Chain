#! /usr/bin/env python
# coding: utf-8

import time
import json
from Robot import Robot_SendMSG
from SM4 import encrypt_ecb



def Message_Send_Const(queue,ip,port,method,key,num):
    send_key = None
    if method == 1:
        method = 'sm4'
        send_key = key
    elif method == 2:
        method = 'sm2'
        send_key = key
    else:
        method = 'plain'
        send_key = None
    message = ' '
    while True:
        if queue.is_empty():
            print 'No message in the queue!'
            pass
        else:
            message = str(queue.get_queue_latest())
        stmp = time.time()
        date = time.localtime(stmp)
        format_time = time.strftime('%Y-%m-%d %H:%M:%S', date)
        json_message = {str(format_time):message,'method':method,'key':send_key,'robot_num':num}
        json_message = json.dumps(json_message)
        udp = Robot_SendMSG(ip=ip,port=port)
        udp.send_msg(json_message)
        print 'Send message successfully!'
        time.sleep(0.5)

def Message_Send(queue,ip,port,method,key,num):
    send_key = None
    if method == 1:
        method = 'sm4'
        send_key = key
    elif method == 2:
        method = 'sm2'
        send_key = key
    else:
        method = 'plain'
        send_key = None
    message = ' '
    if queue.is_empty():
        print 'No message in the queue!'
        time.sleep(1)
        pass
    else:
        message = str(queue.get_queue_latest())
    stmp = time.time()
    date = time.localtime(stmp)
    format_time = time.strftime('%Y-%m-%d %H:%M:%S', date)
    json_message = {str(format_time):message,'method':method,'key':send_key,'robot_num':num}
    json_message = json.dumps(json_message)
    udp = Robot_SendMSG(ip=ip,port=port)
    udp.send_msg(json_message)
    print 'Send message successfully!'

def SM4_Message_Send_Const(queue,ip,port,method,key,num):
    print queue.show_queue()
    send_key = None
    if method == 1:
        method = 'sm4'
        send_key = key
    elif method == 2:
        method = 'sm2'
        send_key = key
    else:
        method = 'plain'
        send_key = None
    message = ' '
    while True:
        if queue.is_empty():
            print 'No message in the queue!'
            time.sleep(1)
            pass
        else:
            message = str(queue.get_queue_latest())
        stmp = time.time()
        date = time.localtime(stmp)
        format_time = time.strftime('%Y-%m-%d %H:%M:%S', date)
        json_message = {str(format_time):message,'method':method,'key':send_key,'robot_num':num}
        crypt_message = encrypt_ecb(str(json_message),key=key)
        json_message = json.dumps({'message':crypt_message,'robot_num':num})
        udp = Robot_SendMSG(ip=ip,port=port)
        udp.send_msg(json_message)
        time.sleep(0.5)
        print 'Send message successfully!'

def SM4_Message_Send(queue,ip,port,method,key,num):
    send_key = None
    if method == 1:
        method = 'sm4'
        send_key = key
    elif method == 2:
        method = 'sm2'
        send_key = key
    else:
        method = 'plain'
        send_key = None
    message = ' '
    if queue.is_empty():
        print 'No message in the queue!'
        time.sleep(1)
        pass
    else:
        message = str(queue.get_queue_latest())
    stmp = time.time()
    date = time.localtime(stmp)
    format_time = time.strftime('%Y-%m-%d %H:%M:%S', date)
    json_message = {str(format_time):message,'method':method,'key':send_key,'robot_num':num}
    crypt_message = encrypt_ecb(str(json_message),key=key)
    json_message = json.dumps({'message':crypt_message,'robot_num':num})
    udp = Robot_SendMSG(ip=ip,port=port)
    udp.send_msg(json_message)
    print 'Send message successfully!'


def read_txt(name,lines,index):
    with open(name,'r') as f:
        context = f.readlines()
    for line in context:
        line =line.strip('\n')
        lines[index].append(line)
    return lines
