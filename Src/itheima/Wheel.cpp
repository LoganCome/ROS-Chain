//
// Created by Kaijun on 2021/1/29.
//

#include <cstdio>
#include "Wheel.h"
#include "config.h"


Wheel::Wheel(Motor *motor, Encoder *encoder, PID *pid) {
    this->motor = motor;
    this->encoder = encoder;
    this->pid = pid;
}

void Wheel::init() {
    this->motor->init();
    this->encoder->init();
}

/**
 * 在轮子的心跳中需要计算轮子实时的转速
 */
void Wheel::tick() {
    // 每隔一定时间来计算
    uint32_t currTime = HAL_GetTick();

    if(currTime - lastUpdateTime  > 50  ){
        float dt = (float)(currTime - lastUpdateTime)/1000.0; // s

        lastUpdateTime = currTime;
        // 读取编码器的数据
        short count = this->encoder->read();

        // 计算轮子当前的转速
        /**
         * 编码器读数:
         * 圈数: 编码器读数/一圈的信号数
         * 轮子走了多远的距离: 圈数* 轮子直径*3.14   m  ===> 编码器读数/一圈的信号数 * 轮子直径*3.14 / 0.050s
         * 时间: 50ms --->  50/1000---> 0.050s
         */

         vel  = double(count)/WHEEL_TPR * WHEEL_DIAMETER * PI / dt;

         /**
          * pid 控制逻辑
          */
          float pwm = this->pid->compute(this->targetVel,this->vel);

//        printf("kp:%d \r\n",int(this->pid->kp*1000));
          this->motor->setPwm(pwm);
//          printf("count:%d vel:%f  pwm:%f targetVel:%f\r\n",count,vel,pwm,targetVel);
    }
}

void Wheel::setVel(float vel) {
    this->targetVel = vel;
}

float Wheel::getVel() {
    return this->vel;
}


Wheel::~Wheel() {
    delete this->motor;
    delete this->encoder;
    delete this->pid;
}