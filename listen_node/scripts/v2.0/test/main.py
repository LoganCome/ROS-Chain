#! /usr/bin/env python
# coding: utf-8
import threading
import Robot
import time
from Send_Control import Message_Send,Message_Send_Const
from sm2 import SM2
import listen
from scripts.v4 import pyargs

message_send_threads = []
sm2 = SM2()
Continue = True

if __name__ == '__main__':



    # fileread_threads[0] = threading.Thread(target=read_txt,args=('./deploy_args/keys.txt',lines,0,))
    # fileread_threads[1] = threading.Thread(target=read_txt,args=('./deploy_args/node_names.txt',lines,1,))
    # fileread_threads[2] = threading.Thread(target=read_txt,args=('./deploy_args/ports.txt',lines,2,))
    # fileread_threads[3] = threading.Thread(target=read_txt,args=('./deploy_args/topic_names.txt',lines,3,))
    # for i in range(4):
    #     fileread_threads[i].start()
    # for i in range(4):
    #     fileread_threads[i].join()
    #
    # for i in range(lines[0].__len__()):
    #     sm4_queue.append(queue(10))
    #     plain_queue.append(queue(10))
    #
    # for i in range(lines[0].__len__()):
    #     monitor_threads.append(threading.Thread(target=monitor_node_sm4crypt,args=(lines[1][i],lines[3][i],sm4_queue[i],lines[0][i],)))
    # for i in range(lines[0].__len__()):
    #     monitor_threads[i].start()
    # for i in range(lines[0].__len__()):
    #     monitor_threads[i].join()
    #
    # for i in range(lines[0].__len__()):
    #     monitor_threads.append(threading.Thread(target=monitor_node_plain_text,args=(lines[1][i],lines[3][i],plain_queue[i],)))
    # for i in range(lines[0].__len__()):
    #     monitor_threads[i].start()
    # for i in range(lines[0].__len__()):
    #     monitor_threads[i].join()
    listen.listen()

    while Continue:
        crypt_method = input("Please enter the method of encryption(sm2=2, sm4=1 or No encryption=3):\n")
        if crypt_method == 1:
            bulk_or_no = input("Please choose whether to encrypt message in bulk(Yes=1 or NO=0), if u want to send constantly, please press 2:\n")
            if bulk_or_no == 1:
                # bot_num = input("Please enter the num of the bot(1,2,3...10)")
                for bot_num in range(pyargs.lines[0].__len__()):
                    # print lines[2][bot_num]
                    Message_Send(pyargs.sm4_queue[bot_num], Robot.Master_URI, int(pyargs.lines[2][bot_num]), 1,
                                 pyargs.lines[0][bot_num], bot_num + 1)
            elif bulk_or_no == 0:
                bot_num = list(input('Please enter the num of the bot(1,2,3...10) u want to encrypt in sm4:\n'))
                len_bot_num = bot_num.__len__()
                len_bots = pyargs.lines[0].__len__()
                bot_plain = []
                for i in range(len_bot_num):
                    Message_Send(pyargs.sm4_queue[bot_num[i] - 1], Robot.Master_URI, int(
                        pyargs.lines[2][bot_num[i] - 1]), 1, pyargs.lines[0][bot_num[i] - 1], bot_num[i])
                for i in range(len_bots):
                    bot_plain.append(i+1)
                for i in range(len_bot_num):
                    if bot_num[i] in bot_plain:
                        bot_plain.remove(bot_num[i])
                for i in range(bot_plain.__len__()):
                    Message_Send(pyargs.plain_queue[bot_plain[i] - 1], Robot.Master_URI, int(
                        pyargs.lines[2][bot_plain[i] - 1]), 3, None, bot_plain[i])
            else:
                print '----------------Warning!----------------\n'
                cons_bulk_or_no = input('You are going to send data constantly!\n Once this process is ongoing, You can only shutdown the main thread!\n which means u can\'t do other opeartions!\n If you want to continue, please choose whether to encrypt message in bulk(Yes=3 or NO=4)\n')
                if cons_bulk_or_no == 3:
                    while True:
                        for bot_num in range(pyargs.lines[0].__len__()):
                            # print lines[2][bot_num]
                            Message_Send(pyargs.sm4_queue[bot_num], Robot.Master_URI, int(pyargs.lines[2][bot_num]), 1,
                                         pyargs.lines[0][bot_num], bot_num + 1)
                        time.sleep(0.5)
                else:
                    bot_num = list(input('Please enter the num of the bot(1,2,3...10) u want to encrypt in sm4:\n'))
                    len_bot_num = bot_num.__len__()
                    len_bots = pyargs.lines[0].__len__()
                    bot_plain = []
                    # t1 = threading.Thread(target=Message_Send,args=(sm4_queue[bot_num[i]-1], Robot.Master_URI, int(lines[2][bot_num[i]-1]),1,))
                    for i in range(len_bot_num):
                        # Message_Send(sm4_queue[bot_num[i]-1], Robot.Master_URI, int(lines[2][bot_num[i]-1]),1)
                        message_send_threads.append(threading.Thread(target=Message_Send_Const, args=(
                        pyargs.sm4_queue[bot_num[i] - 1], Robot.Master_URI, int(pyargs.lines[2][bot_num[i] - 1]), 1,
                        pyargs.lines[0][bot_num[i] - 1], bot_num[i],)))
                    for i in range(len_bots):
                        bot_plain.append(i+1)
                    for i in range(len_bot_num):
                        if bot_num[i] in bot_plain:
                            bot_plain.remove(bot_num[i])
                    for i in range(bot_plain.__len__()):
                        # Message_Send(plain_queue[bot_plain[i]-1], Robot.Master_URI, int(lines[2][bot_plain[i]-1]),3)
                        message_send_threads.append(threading.Thread(target=Message_Send_Const, args=(
                        pyargs.plain_queue[bot_plain[i] - 1], Robot.Master_URI, int(pyargs.lines[2][bot_plain[i] - 1]), 3, None, bot_plain[i],)))
                    for i in range(len(message_send_threads)):
                        message_send_threads[i].start()
                    for i in range(len(message_send_threads)):
                        message_send_threads[i].join()
        if crypt_method == 2:
            bulk_or_no = input("Please choose whether to encrypt message in bulk(Yes=1 or NO=0), if u want to send constantly, please press 2:\n")
            if bulk_or_no == 1:
                # bot_num = input("Please enter the num of the bot(1,2,3...10)")
                for bot_num in range(pyargs.lines[0].__len__()):
                    key = list(sm2.key_produce())
                    # print lines[2][bot_num]
                    Message_Send(pyargs.plain_queue[bot_num], Robot.Master_URI, int(pyargs.lines[2][bot_num]), 2, key, bot_num + 1)
            elif bulk_or_no == 0:
                bot_num = list(input('Please enter the num of the bot(1,2,3...10) u want to encrypt in sm2:\n'))
                len_bot_num = bot_num.__len__()
                len_bots = pyargs.lines[0].__len__()
                bot_plain = []
                for i in range(len_bot_num):
                    key = list(sm2.key_produce())
                    Message_Send(pyargs.plain_queue[bot_num[i] - 1], Robot.Master_URI, int(
                        pyargs.lines[2][bot_num[i] - 1]), 2, key, bot_num[i])
                for i in range(len_bots):
                    bot_plain.append(i+1)
                for i in range(len_bot_num):
                    if bot_num[i] in bot_plain:
                        bot_plain.remove(bot_num[i])
                for i in range(bot_plain.__len__()):
                    Message_Send(pyargs.plain_queue[bot_plain[i] - 1], Robot.Master_URI, int(
                        pyargs.lines[2][bot_plain[i] - 1]), 3, None, bot_plain[i])
            else:
                print '----------------Warning!----------------\n'
                cons_bulk_or_no = input('You are going to send data constantly!\n Once this process is ongoing, You can only shutdown the main thread!\n which means u can\'t do other opeartions!\n If you want to continue, please choose whether to encrypt message in bulk(Yes=3 or NO=4)\n')
                if cons_bulk_or_no == 3:
                    keys = []
                    for i in range(pyargs.lines[0].__len__()):
                        key = sm2.key_produce()
                        keys.append(key)
                    while True:
                        for bot_num in range(pyargs.lines[0].__len__()):
                            # print lines[2][bot_num]
                            Message_Send(pyargs.plain_queue[bot_num], Robot.Master_URI, int(pyargs.lines[2][bot_num]), 2, keys[bot_num], bot_num + 1)
                        time.sleep(0.5)
                else:
                    bot_num = list(input('Please enter the num of the bot(1,2,3...10) u want to encrypt in sm2:\n'))
                    len_bot_num = bot_num.__len__()
                    len_bots = pyargs.lines[0].__len__()
                    bot_plain = []
                    keys = []
                    for i in range(bot_num.__len__()):
                        key = sm2.key_produce()
                        keys.append(key)
                    # t1 = threading.Thread(target=Message_Send,args=(sm4_queue[bot_num[i]-1], Robot.Master_URI, int(lines[2][bot_num[i]-1]),1,))
                    for i in range(len_bot_num):
                        # Message_Send(sm4_queue[bot_num[i]-1], Robot.Master_URI, int(lines[2][bot_num[i]-1]),1)
                        message_send_threads.append(threading.Thread(target=Message_Send_Const, args=(
                        pyargs.plain_queue[bot_num[i] - 1], Robot.Master_URI, int(pyargs.lines[2][bot_num[i] - 1]), 2, keys[i], bot_num[i],)))
                    for i in range(len_bots):
                        bot_plain.append(i+1)
                    for i in range(len_bot_num):
                        if bot_num[i] in bot_plain:
                            bot_plain.remove(bot_num[i])
                    for i in range(bot_plain.__len__()):
                        # Message_Send(plain_queue[bot_plain[i]-1], Robot.Master_URI, int(lines[2][bot_plain[i]-1]),3)
                        message_send_threads.append(threading.Thread(target=Message_Send_Const, args=(
                        pyargs.plain_queue[bot_plain[i] - 1], Robot.Master_URI, int(pyargs.lines[2][bot_plain[i] - 1]), 3, None, bot_plain[i],)))
                    for i in range(len(message_send_threads)):
                        message_send_threads[i].start()
                    for i in range(len(message_send_threads)):
                        message_send_threads[i].join()

        if crypt_method == 3:
            bulk_or_no = input("Please choose whether to send message constantly(Constantly=2 or No_Constant=1):\n")
            if bulk_or_no == 1:
                # bot_num = input("Please enter the num of the bot(1,2,3...10)")
                for bot_num in range(pyargs.lines[0].__len__()):
                    # print lines[2][bot_num]
                    Message_Send(pyargs.plain_queue[bot_num], Robot.Master_URI, int(pyargs.lines[2][bot_num]), 3, None, bot_num + 1)
            else:
                print '----------------Warning!----------------\n'
                cons_bulk_or_no = input('You are going to send data constantly!\n Once this process is ongoing, You can only shutdown the main thread!\n which means u can\'t do other opeartions!\n If you want to continue, please press 3:\n')
                if cons_bulk_or_no == 3:
                    while True:
                        for bot_num in range(pyargs.lines[0].__len__()):
                            # print lines[2][bot_num]
                            Message_Send(pyargs.plain_queue[bot_num], Robot.Master_URI, int(pyargs.lines[2][bot_num]), 3, None, bot_num + 1)
                        time.sleep(0.5)
                else:
                    pass

        Continue = input("Do you want to continue?(Yes=1 or No=0)\n")
        if Continue == 1:
            time.sleep(1)
            pass
        else:
            Continue = False
