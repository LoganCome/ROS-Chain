//
// Created by Kaijun on 2021/1/29.
//

#include <cstdio>
#include "PID.h"


PID::PID(float kp, float ki, float kd) {
    this->kp = kp;
    this->ki = ki;
    this->kd = kd;
}

float PID::compute(float target, float current) {
    // 计算误差
    float error = target - current;
    // 误差的累计
    intergral +=error;
    // 本次误差和上一次误差的差异
    derivative = error - prevError;

    // 套用pid的公式
    pwm += kp*error + ki*intergral + kd*derivative;

    // 记录上一次的误差
    prevError = error;

    return pwm;
}

void PID::reset(){
    pwm = 0;
    intergral = 0;
    derivative = 0;
    prevError = 0;
}