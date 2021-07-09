//
// Created by Kaijun on 2021/1/29.
//

#ifndef HEIMAROBOTV6_WHEEL_H
#define HEIMAROBOTV6_WHEEL_H

#include "Motor.h"
#include "Encoder.h"
#include "PID.h"

class Wheel {
public:
    Wheel(Motor* motor,Encoder* encoder,PID* pid);
    ~Wheel();

    Motor* motor;
    Encoder* encoder;
    PID* pid;

    // 对外输出轮子转速
    float vel=0;
    /**
     * 目标速度
     */
     float targetVel=0;

     uint32_t lastUpdateTime;
public:
    void init();
    // 轮子的心跳
    void tick();
    /**
     * 设置轮子的转速  m/s
     */
     void setVel(float vel);
     float getVel();
};


#endif //HEIMAROBOTV6_WHEEL_H
