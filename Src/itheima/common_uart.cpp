//
// Created by Kaijun on 2020/9/13.
//

#include "common_uart.h"
//
// Created by Kaijun on 2020/6/8.
//
#include "common_uart.h"
#include <string.h>
#include <stdio.h>
#include "stm32f1xx_hal.h"

// 定义缓冲区的大小
const uint32_t BUFFER_SIZE = 255;
// 声明缓冲区
static uint8_t uart_rx_buff[255];
/**
 * 串口的初始化操作
 */
void common_uart_init(){
    // 开启DMA接收数据
    HAL_UART_Receive_DMA(&huart1, (uint8_t*)uart_rx_buff, BUFFER_SIZE);
    // 开启空闲中断处理
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
    // 开启通信异常处理
    __HAL_UART_ENABLE_IT(&huart1,UART_IT_ERR);
}

/**
 * 需要将这个函数放到stm32f1xx_it.c 文件中的void USART1_IRQHandler(void)函数中调用
 * @param huart1
 */
void common_uart_idle_handle(UART_HandleTypeDef* huart1){

    if(USART1 == huart1->Instance)
    {	// 判断是否是空闲中断
        if (RESET != __HAL_UART_GET_FLAG(huart1, UART_FLAG_IDLE)) {
            // 计算接收到的数据长度
            uint32_t data_length = BUFFER_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart1_rx);
            __HAL_UART_CLEAR_IDLEFLAG(huart1);
            // 停止本次DMA传输
            HAL_UART_DMAStop(huart1);
            // 将数据丢给外部去处理
            while (HAL_UART_Receive_DMA(huart1, uart_rx_buff, BUFFER_SIZE) != HAL_OK) {
                huart1->RxState = HAL_UART_STATE_READY;
                __HAL_UNLOCK(huart1);
            }

            // 数据处理的回调
            common_uart_idle_callback(uart_rx_buff,data_length);
        }
    }
}

/**
 * 将数据使用DMA的方式发送出去
 * @param data
 * @param size
 */
void common_uart_send(uint8_t* data ,uint16_t size){
    // 直到DMA空闲,才进行数据的发送
/*    while (HAL_UART_GetState(&huart1) == HAL_UART_STATE_BUSY_TX_RX);

    while (HAL_UART_Transmit_DMA(&huart1, data, size) != HAL_OK);*/
    while (HAL_UART_GetState(&huart1) == HAL_UART_STATE_BUSY_TX_RX);
    auto *buf = new uint8_t[size];
    memcpy(buf, data, size);
    while (HAL_UART_Transmit_DMA(&huart1, data, size) != HAL_OK);
    delete[] buf;
}


/* 中断错误处理函数，在此处理overrun错误 */
void HAL_UART_ErrorCallback1(UART_HandleTypeDef *huart)
{
    if(__HAL_UART_GET_FLAG(huart,UART_FLAG_PE) != RESET)
    {
        __HAL_UART_CLEAR_OREFLAG(huart);
    }

    if(__HAL_UART_GET_FLAG(huart,UART_FLAG_FE) != RESET)
    {
        __HAL_UART_CLEAR_FEFLAG(huart);
    }
    if(__HAL_UART_GET_FLAG(huart,UART_FLAG_NE) != RESET)
    {
        __HAL_UART_CLEAR_NEFLAG(huart);
    }
    if(__HAL_UART_GET_FLAG(huart,UART_FLAG_ORE) != RESET)
    {
        __HAL_UART_CLEAR_OREFLAG(huart);
    }

    common_uart_init();
}