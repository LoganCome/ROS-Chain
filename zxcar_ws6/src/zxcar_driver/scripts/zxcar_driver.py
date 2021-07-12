#!/usr/bin/env python
#encoding:utf-8
import rospy
import serial
import threading
import struct
import Queue
import time
from geometry_msgs.msg import Twist
from sensor_msgs.msg import Imu,MagneticField

"""
CE   head0
FA   head1
01   type 类型 
1E   长度 30
05 0C 温度  低八位 高八位
      
B8 FF 三轴加速度
98 02 
C0 42 

B8 FF 三轴角速度
98 02
C0 42

B8 FF  三轴地磁数据
98 02
C0 42

00 00 线速度

00 00 角速度

AD 00 帧尾  低八位 高八位
"""
# print(0x00<<8 | 0xAd)
# print(0x0c<<8 | 0x05)
class ZxcarDriver():

    def __init__(self):
        rospy.init_node("zxcardriver")
        rospy.on_shutdown(self.onShutDown)


        port = rospy.get_param("~port","/dev/ttyUSB2");
        print("port===",port);
        baudrate = rospy.get_param("~baudrate",115200);

        # 开启串口
        self.ser = serial.Serial(port=port,baudrate=baudrate);
        flag =self.ser.isOpen()
        print("serial open success..",flag)

        # 创建用于接收和解析缓冲的队列
        self.recv_queue = Queue.Queue()



        # 订阅消息cmd_vel
        rospy.Subscriber("/cmd_vel",Twist,self.cmdvel_callback);
        # 发布消息
        self.velPublisher = rospy.Publisher("/zxcar/get_vel",Twist,queue_size=50)
        # 发布IMU数据
        self.imuPublisher = rospy.Publisher("/zxcar/imu",Imu,queue_size=50)
        # 地磁数据
        self.magPublishuer = rospy.Publisher("/zxcar/magnetic",MagneticField,queue_size=50)


        threading.Thread(target=self.receive_data).start();
        threading.Thread(target=self.parse_thread).start();

        # 让节点运行起来
        rospy.spin();

    def cmdvel_callback(self,msg):
        v = msg.linear.x
        w = msg.angular.z
        self.send_vel(v,w);


    def onShutDown(self):
       self.send_vel(0,0)

       twist = Twist()
       twist.linear.x = 0
       twist.linear.y = 0
       twist.angular.z = 0
       self.velPublisher.publish(twist);


    def send_vel(self,vel,angular):
        # 0xce, 0xfa, 0x02 10 00 00 00 00 ad 00
        cmd = [0xce, 0xfa, 0x02]
        cmd.append(10);
        # 速度放大1000倍,避免通过小数传输
        linear_pack = bytearray(struct.pack('h', int(vel * 1000)));
        angular_pack = bytearray(struct.pack('h', int(angular * 1000)));

        cmd.append(linear_pack[0])
        cmd.append(linear_pack[1])

        cmd.append(angular_pack[0])
        cmd.append(angular_pack[1])
        cmd.append(0xAD)
        cmd.append(0x00)

        self.ser.write(cmd)

    def parse_data(self,data):

        if len(data)<4:
            return;
        # 判断帧头
        if data[0]==0xce and data[1]==0xfa:
            data_type = data[2]
            data_length = data[3]

            if data_length != len(data):
                return

            # 解析数据
            if data_type == 0x01:
                temp = struct.unpack('h', bytearray(data[4:6]))[0]
                # acc

                ax = struct.unpack('h', bytearray(data[6:8]))[0]
                ay = struct.unpack('h', bytearray(data[8:10]))[0]
                az = struct.unpack('h', bytearray(data[10:12]))[0]

                # rot

                gx = struct.unpack('h', bytearray(data[12:14]))[0]
                gy = struct.unpack('h', bytearray(data[14:16]))[0]
                gz = struct.unpack('h', bytearray(data[16:18]))[0]

                # mag
                mx = struct.unpack('h', bytearray(data[18:20]))[0]
                my = struct.unpack('h', bytearray(data[20:22]))[0]
                mz = struct.unpack('h', bytearray(data[22:24]))[0]

                v = struct.unpack('h', bytearray(data[24:26]))[0]
                w = struct.unpack('h', bytearray(data[26:28]))[0]

                code = struct.unpack('h', bytearray(data[28:30]))[0]

                if code == 0xAd:
                    # 发布速度信息
                    twist = Twist()
                    twist.linear.x = v/1000.0
                    twist.angular.z = w/1000.0
                    self.velPublisher.publish(twist)

                    # 发布IMU信息
                    imu = Imu()
                    accel_ratio =  1/16384.0
                    imu.linear_acceleration.x = ax * accel_ratio
                    imu.linear_acceleration.y = ay * accel_ratio
                    imu.linear_acceleration.z = az * accel_ratio

                    gyro_ratio = 0.00026644#1/65.5/(180/3.1415926)
                    imu.angular_velocity.x = gx * gyro_ratio;
                    imu.angular_velocity.y = gy * gyro_ratio;
                    imu.angular_velocity.z = gz * gyro_ratio;
                    self.imuPublisher.publish(imu)

                    mag = MagneticField()
                    mag.magnetic_field.x = mx
                    mag.magnetic_field.y = my
                    mag.magnetic_field.z = mz
                    self.magPublishuer.publish(mag)

                    # print("temp={} a=({}, {}, {}) g=({}, {}, {}) m=({}, {}, {}) v={}  w={}".format(temp, ax, ay, az, gx, gy,
                    #                                                                            gz, mx, my, mz, v, w))
                else:
                    print("帧尾校验失败")
    def parse_thread(self):
        while True:
            if rospy.is_shutdown():
                break;
            data = self.recv_queue.get();
            self.parse_data(data);

    def receive_data(self):
        data = bytearray([])
        while True:
            if rospy.is_shutdown():
                break;
            # 获取缓冲区内的数据大小
            n = self.ser.inWaiting();
            if n > 0:
                data += self.ser.read(n);

            if len(data) > 0 and n == 0:
                """说明刚好读完了一帧数据"""
                self.recv_queue.put(data);
                # 解析数据
                #self.parse_data(data);
                data = bytearray([])

if __name__ == '__main__':
    ZxcarDriver()