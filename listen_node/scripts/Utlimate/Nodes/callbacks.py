'''
回调函数封装
'''
from nav_msgs.msg import Odometry
from sensor_msgs.msg import Image
from sensor_msgs.msg import Imu
from sensor_msgs.msg import CompressedImage
from scripts.history.v4.msg_parser.OdometryParser import OdometryParser
import redis

def Connect_redis(topics):
    pool = redis.ConnectionPool(host='127.0.0.1', port=6379, deploy_contract=True,max_connections=5)
    r = redis.Redis(connection_pool=pool)

    for i in range(len(topics)):
        r.rpush(str(topics[i]),0)
    print('Connection established!')
    return  r


def Odometry_callback_func(msg,redis,topics):
    if not isinstance(msg,Odometry):
        raise TypeError("Wrong message Types!")

    parser = OdometryParser('Odometry')
    msg = parser.parse(msg,topics)

    redis.rpush(topics,msg)

def Image_callback_func(msg,redis,topics):
    if not isinstance(msg,Image):
        raise TypeError("Wrong message Types!")

    pass

def Imu_callback_func(msg,redis,topics):
    if not isinstance(msg,Imu):
        raise TypeError("Wrong message Types!")

    pass

def CompressedImage_callback_func(msg,redis,topics):
    if not isinstance(msg,CompressedImage):
        raise TypeError("Wrong message Types!")

    pass