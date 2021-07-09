//
// Created by Kaijun on 2021/1/29.
//

#ifndef HEIMAROBOTV6_ENCODER_H
#define HEIMAROBOTV6_ENCODER_H


#include <stm32f1xx_hal.h>

class Encoder {
public:
    /**
     * @param tim   ��ʱ��
     * @param channel   ͨ��
     * @param direction  ���ڵ��������
     */
    Encoder(TIM_HandleTypeDef *tim,uint32_t channel,int direction);
    ~Encoder();

    TIM_HandleTypeDef *tim;
    uint32_t channel;
    int direction;

    /**
     * ��ʼ��
     */
    void init();
    /**
     * @return ��ȡ����������
     */
    short read();
};


#endif //HEIMAROBOTV6_ENCODER_H
