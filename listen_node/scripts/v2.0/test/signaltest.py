#! /usr/bin/env python
# coding: utf-8

import signal
Constant_send = True
def sigint_handler(signum, frame):
    print("The thread of receiving data will be killed!")
    #将全局的线程开关控制变量设False，即关闭线程
    global Constant_send
    Constant_send = False
    print("The killing is going on.")
if __name__ == '__main__':
    while Constant_send:
        print 'menghaoshishab'
    signal.signal(signal.SIGINT, sigint_handler)

