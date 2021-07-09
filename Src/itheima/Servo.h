//
// Created by Kaijun on 2021/1/29.
//

#ifndef HEIMAROBOTV6_SERVO_H
#define HEIMAROBOTV6_SERVO_H


class Servo {
public:
    Servo(TIM_HandleTypeDef *tim,uint32_t channel);
    ~Servo();

private:
    TIM_HandleTypeDef *tim;
    uint32_t channel;

public:
    void init();
    /**
     * ���ƶ��ת��
     * @param rad
     */
    void setRadian(double rad);
    /**
     * ����PWM
     * @param pwm
     */
    void setPwm(int pwm);
};


#endif //HEIMAROBOTV6_SERVO_H
