//
// Created by Kaijun on 2021/1/29.
//

#include "Encoder.h"

Encoder::Encoder(TIM_HandleTypeDef *tim, uint32_t channel, int direction) {
    this->tim = tim;
    this->channel = channel;
    this->direction = direction;
}

void Encoder::init(){
    // ������ʱ��
    HAL_TIM_Encoder_Start(this->tim,this->channel);
}

short Encoder::read(){
    // ��ȡ������
    short count = __HAL_TIM_GET_COUNTER(this->tim);
    // ���㷽��
    count *= this->direction;
    //  ����֮���������
    __HAL_TIM_SET_COUNTER(this->tim,0);
    return count;
}


Encoder::~Encoder() {

}