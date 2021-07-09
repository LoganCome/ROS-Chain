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
 * �����ӵ���������Ҫ��������ʵʱ��ת��
 */
void Wheel::tick() {
    // ÿ��һ��ʱ��������
    uint32_t currTime = HAL_GetTick();

    if(currTime - lastUpdateTime  > 50  ){
        float dt = (float)(currTime - lastUpdateTime)/1000.0; // s

        lastUpdateTime = currTime;
        // ��ȡ������������
        short count = this->encoder->read();

        // �������ӵ�ǰ��ת��
        /**
         * ����������:
         * Ȧ��: ����������/һȦ���ź���
         * �������˶�Զ�ľ���: Ȧ��* ����ֱ��*3.14   m  ===> ����������/һȦ���ź��� * ����ֱ��*3.14 / 0.050s
         * ʱ��: 50ms --->  50/1000---> 0.050s
         */

         vel  = double(count)/WHEEL_TPR * WHEEL_DIAMETER * PI / dt;

         /**
          * pid �����߼�
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