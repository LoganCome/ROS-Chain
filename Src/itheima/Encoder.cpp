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
    // 启动定时器
    HAL_TIM_Encoder_Start(this->tim,this->channel);
}

short Encoder::read(){
    // 读取编码器
    short count = __HAL_TIM_GET_COUNTER(this->tim);
    // 计算方向
    count *= this->direction;
    //  读完之后清空数据
    __HAL_TIM_SET_COUNTER(this->tim,0);
    return count;
}


Encoder::~Encoder() {

}