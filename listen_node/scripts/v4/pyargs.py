fileread_threads = [None]*5
monitor_threads = []
lines = [[] for i in range(5)]
sm4_queue = []
plain_queue = []


CRYPT_END_SEND_TARGET_IP = '127.0.0.1'
CRYPT_END_SM2andSM4_SEND_PORT = 8871
CRYPT_END_RECV_PORT = 8870

CRYPT_END_CRIPTMESSAGE_TARGET_IP = '127.0.0.1'

MASTER_IP='127.0.0.1'