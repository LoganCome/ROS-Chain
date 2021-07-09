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

// ���建�����Ĵ�С
const uint32_t BUFFER_SIZE = 255;
// ����������
static uint8_t uart_rx_buff[255];
/**
 * ���ڵĳ�ʼ������
 */
void common_uart_init(){
    // ����DMA��������
    HAL_UART_Receive_DMA(&huart1, (uint8_t*)uart_rx_buff, BUFFER_SIZE);
    // ���������жϴ���
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
    // ����ͨ���쳣����
    __HAL_UART_ENABLE_IT(&huart1,UART_IT_ERR);
}

/**
 * ��Ҫ����������ŵ�stm32f1xx_it.c �ļ��е�void USART1_IRQHandler(void)�����е���
 * @param huart1
 */
void common_uart_idle_handle(UART_HandleTypeDef* huart1){

    if(USART1 == huart1->Instance)
    {	// �ж��Ƿ��ǿ����ж�
        if (RESET != __HAL_UART_GET_FLAG(huart1, UART_FLAG_IDLE)) {
            // ������յ������ݳ���
            uint32_t data_length = BUFFER_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart1_rx);
            __HAL_UART_CLEAR_IDLEFLAG(huart1);
            // ֹͣ����DMA����
            HAL_UART_DMAStop(huart1);
            // �����ݶ����ⲿȥ����
            while (HAL_UART_Receive_DMA(huart1, uart_rx_buff, BUFFER_SIZE) != HAL_OK) {
                huart1->RxState = HAL_UART_STATE_READY;
                __HAL_UNLOCK(huart1);
            }

            // ���ݴ���Ļص�
            common_uart_idle_callback(uart_rx_buff,data_length);
        }
    }
}

/**
 * ������ʹ��DMA�ķ�ʽ���ͳ�ȥ
 * @param data
 * @param size
 */
void common_uart_send(uint8_t* data ,uint16_t size){
    // ֱ��DMA����,�Ž������ݵķ���
/*    while (HAL_UART_GetState(&huart1) == HAL_UART_STATE_BUSY_TX_RX);

    while (HAL_UART_Transmit_DMA(&huart1, data, size) != HAL_OK);*/
    while (HAL_UART_GetState(&huart1) == HAL_UART_STATE_BUSY_TX_RX);
    auto *buf = new uint8_t[size];
    memcpy(buf, data, size);
    while (HAL_UART_Transmit_DMA(&huart1, data, size) != HAL_OK);
    delete[] buf;
}


/* �жϴ����������ڴ˴���overrun���� */
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