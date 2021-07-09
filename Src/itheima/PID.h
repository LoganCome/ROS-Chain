//
// Created by Kaijun on 2021/1/29.
//

#ifndef HEIMAROBOTV6_PID_H
#define HEIMAROBOTV6_PID_H


class PID {
public:
    PID(float kp,float ki,float kd);
    float kp;
    float ki;
    float kd;

    float pwm=0;
    // ��һ�ε����
    float prevError;
    // ����
    float intergral;
    // ΢��
    float derivative;
    /**
     * pid�ļ��㺯��
     * @param target  Ŀ��ֵ
     * @param current ��ǰֵ
     * @return  pwm
     */
    float compute(float target,float current);
    /**
     *  �������е����: �����õ��ٶ� �� ��һ�β�һ��
     */
    void reset();

};


#endif //HEIMAROBOTV6_PID_H
