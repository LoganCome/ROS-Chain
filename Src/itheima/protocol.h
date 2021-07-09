//
// Created by Kaijun on 2021/1/29.
//

#ifndef HEIMAROBOTV6_PROTOCOL_H
#define HEIMAROBOTV6_PROTOCOL_H

/**
 * 协议文件
 * 协议头
 * 协议类型
 * 协议的长度
 * 温度
 * 三轴加速度
 * 三轴角速度
 * 三轴地磁
 * 线速度
 * 角速度
 * 协议结尾
 */
#define FLAG_HEAD0 0xce
#define FLAG_HEAD1 0xfa
#define FLAG_TAIL 0xad

#define TYPE_UPLOAD 0x01;
#define TYPE_RECEIVE 0x02;

#pragma pack(1)
 struct TXProtocol{
     // 帧头信息
     uint8_t head0;
     uint8_t head1;
     uint8_t type;
     uint8_t len;

     short temperature;

     short ax;
     short ay;
     short az;

     short gx;
     short gy;
     short gz;

     short mx;
     short my;
     short mz;

     short velocity;
     short angular;
     // 协议尾部
     short code;

 };



#endif //HEIMAROBOTV6_PROTOCOL_H
