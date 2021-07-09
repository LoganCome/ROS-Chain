//
// Created by Kaijun on 2021/1/29.
//
#include "Encoder.h"
#include "Servo.h"
#include "config.h"
#include "Ackermann.h"
#include <math.h>

Ackermann::Ackermann() {
    float kp = 500;
    float ki = 0;
    float kd = 100;
    // 初始化左边轮子 1号
    Motor* motor1 = new Motor(MOTOR1_TIM,MOTOR1_CHANNEL,MOTOR1_A_GPIO_PORT,MOTOR1_A_PIN,MOTOR1_B_GPIO_PORT,MOTOR1_B_PIN,MOTOR1_DIRECTION);
    Encoder* encoder1 = new Encoder(ENCODER1_TIM,ENCODER1_CHANNEL,ENCODER1_DIRECTION);
    PID* pid1 = new PID(kp,ki,kd);
    wheel1 = new Wheel(motor1,encoder1,pid1);

    // 初始化右边轮子 2号
    Motor* motor2 = new Motor(MOTOR2_TIM,MOTOR2_CHANNEL,MOTOR2_A_GPIO_PORT,MOTOR2_A_PIN,MOTOR2_B_GPIO_PORT,MOTOR2_B_PIN,MOTOR2_DIRECTION);
    Encoder* encoder2 = new Encoder(ENCODER2_TIM,ENCODER2_CHANNEL,ENCODER2_DIRECTION);
    PID* pid2 = new PID(kp,ki,kd);
    wheel2 = new Wheel(motor2,encoder2,pid2);

    // 初始化舵机
    servo = new Servo(SERVO1_TIM,SERVO1_CHANNEL);
}

void Ackermann::init() {
    wheel1->init();
    wheel2->init();
    servo->init();
}

void Ackermann::tick() {
    wheel1->tick();
    wheel2->tick();
}

void Ackermann::updateVel(float vel, float rad) {
    servo->setRadian(rad);

    float vel1 = vel * (1 - WHEEL_DISTANCE*tan(rad)/(2*WHEEL_AXIS));
    float vel2 = vel * (1 + WHEEL_DISTANCE*tan(rad)/(2*WHEEL_AXIS));

    wheel1->setVel(vel1);
    wheel2->setVel(vel2);
}

float Ackermann::getVel() {
    float vel1 = wheel1->getVel();
    float vel2 = wheel2->getVel();

    return (vel1 + vel2)/2;
}

float Ackermann::getAngularVel() {
    float vel1 = wheel1->getVel();
    float vel2 = wheel2->getVel();

    return (-vel1 + vel2)/(2*WHEEL_DISTANCE*0.5);
}

Ackermann::~Ackermann() {
    delete this->wheel1;
    delete this->wheel2;
    delete this->servo;
}