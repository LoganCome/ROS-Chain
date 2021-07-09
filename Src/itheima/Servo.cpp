//
// Created by Kaijun on 2021/1/29.
//
#include "config.h"
#include "Servo.h"
#include <math.h>

Servo::Servo(TIM_HandleTypeDef *tim, uint32_t channel) {
    this->tim = tim;
    this->channel = channel;
}

void Servo::init() {
    // 启动定时器
    HAL_TIM_PWM_Start(this->tim,this->channel);

    setRadian(0);
}

void Servo::setPwm(int pwm) {
    __HAL_TIM_SET_COMPARE(this->tim,this->channel,abs(pwm));
}

void Servo::setRadian(double rad) {
    // 根据弧度 计算PWM
    int pwm = SERVO_INIT - rad*SERVO_K;
    setPwm(pwm);
}

Servo::~Servo() {

}