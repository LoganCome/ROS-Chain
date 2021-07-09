//
// Created by Kaijun on 2021/1/29.
//

#ifndef HEIMAROBOTV6_PROTOCOL_H
#define HEIMAROBOTV6_PROTOCOL_H

/**
 * Э���ļ�
 * Э��ͷ
 * Э������
 * Э��ĳ���
 * �¶�
 * ������ٶ�
 * ������ٶ�
 * ����ش�
 * ���ٶ�
 * ���ٶ�
 * Э���β
 */
#define FLAG_HEAD0 0xce
#define FLAG_HEAD1 0xfa
#define FLAG_TAIL 0xad

#define TYPE_UPLOAD 0x01;
#define TYPE_RECEIVE 0x02;

#pragma pack(1)
 struct TXProtocol{
     // ֡ͷ��Ϣ
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
     // Э��β��
     short code;

 };



#endif //HEIMAROBOTV6_PROTOCOL_H
