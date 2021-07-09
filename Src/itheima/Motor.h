//
// Created by Kaijun on 2021/1/29.
//

#ifndef HEIMAROBOTV6_MOTOR_H
#define HEIMAROBOTV6_MOTOR_H


#include <stm32f1xx_hal.h>

class Motor {
private:
    //定时器
    TIM_HandleTypeDef *tim;
    //channel
    uint32_t channel;
    //gpioA
    GPIO_TypeDef *portA;
    uint16_t pinA;
    //gpioB
    GPIO_TypeDef *portB;
    uint16_t pinB;
    //方向校正系数 1: -1:
    int direction = 1;
public:
    Motor(TIM_HandleTypeDef *htim,uint32_t channel,GPIO_TypeDef *portA,uint16_t pinA,GPIO_TypeDef *portB,uint16_t pinB,int direction);

    ~Motor();
    //初始化方法
    void init();
    void setPwm(int pwm);
};


#endif //HEIMAROBOTV6_MOTOR_H
