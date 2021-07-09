//
// Created by Kaijun on 2021/1/29.
//

#include "Motor.h"
#include "config.h"
#include <math.h>
#include <cstdio>

Motor::Motor(TIM_HandleTypeDef *htim, uint32_t channel, GPIO_TypeDef *portA, uint16_t pinA, GPIO_TypeDef *portB,
             uint16_t pinB, int direction) {
    this->tim = htim;
    this->channel = channel;
    this->portA = portA;
    this->pinA = pinA;
    this->portB = portB;
    this->pinB = pinB;

    this->direction = direction;
}

void Motor::init() {
    // 初始化GPIO
    HAL_GPIO_WritePin(this->portA,this->pinA,GPIO_PIN_RESET);
    HAL_GPIO_WritePin(this->portB,this->pinB,GPIO_PIN_RESET);
    // 初始化定时器
    HAL_TIM_PWM_Start(this->tim,this->channel);
    __HAL_TIM_SET_COMPARE(this->tim,this->channel,0);
}
void Motor::setPwm(int pwm) {

    if(pwm > MAX_PWM){
        pwm = MAX_PWM;
    } else if(pwm < MIN_PWM){
        pwm = MIN_PWM;
    }

    pwm *= this->direction;

    if(pwm > 0){ // 轮子正转
        HAL_GPIO_WritePin(this->portB,this->pinB,GPIO_PIN_RESET);
        HAL_GPIO_WritePin(this->portA,this->pinA,GPIO_PIN_SET);
    }else if(pwm <0){
        HAL_GPIO_WritePin(this->portA,this->pinA,GPIO_PIN_RESET);
        HAL_GPIO_WritePin(this->portB,this->pinB,GPIO_PIN_SET);
    }else{
        HAL_GPIO_WritePin(this->portB,this->pinB,GPIO_PIN_RESET);
        HAL_GPIO_WritePin(this->portA,this->pinA,GPIO_PIN_RESET);
    }


    __HAL_TIM_SET_COMPARE(this->tim,this->channel,abs(pwm));
}

Motor::~Motor() {
    printf("motor ~motor..\r\n");
}