import multiprocessing

sm2_robot_num = 0
REDIS_HOST = '127.0.0.1'
redis_port = 6379
queue = multiprocessing.Manager().Queue()

DATA_RECV_IP = '127.0.0.1'

DECRYT_END_SEND_TARGER_IP = '127.0.0.1'
DECRYT_END_SM2andSM4_SEND_PORT = 8870
DECRYT_END_RECV_PORT = 8871