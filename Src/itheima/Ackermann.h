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
     * @param vel  线速度
     * @param rad  弧度
     */
    void updateVel(float vel,float rad);

    float getVel();
    float getAngularVel();
};


#endif //HEIMAROBOTV6_ACKERMANN_H
