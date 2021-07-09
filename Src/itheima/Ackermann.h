//
// Created by Kaijun on 2021/1/29.
//

#ifndef HEIMAROBOTV6_ACKERMANN_H
#define HEIMAROBOTV6_ACKERMANN_H

#include "Wheel.h"
#include "Servo.h"

class Ackermann {
public:
    Ackermann();
    ~Ackermann();

    Servo* servo;
    Wheel* wheel1;
    Wheel* wheel2;

public:
    void init();
    void tick();
    /**
     * @param vel  ���ٶ�
     * @param rad  ����
     */
    void updateVel(float vel,float rad);

    float getVel();
    float getAngularVel();
};


#endif //HEIMAROBOTV6_ACKERMANN_H
