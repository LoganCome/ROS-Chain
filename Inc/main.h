/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2019 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
#define UART_RX_BUF_SIZE		256
#define UART_TX_BUF_SIZE		256
extern volatile uint8_t ucUartRecvEndCounter;
extern volatile uint8_t ucUart1RxBufLen;
extern uint8_t pucUart1RxBuf[UART_RX_BUF_SIZE];
extern uint8_t pucUart1RxBufCache[UART_RX_BUF_SIZE];
extern TIM_HandleTypeDef* usTimer;
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED_RED_Pin GPIO_PIN_13
#define LED_RED_GPIO_Port GPIOC
#define LED_GREEN_Pin GPIO_PIN_14
#define LED_GREEN_GPIO_Port GPIOC
#define LED_BLUE_Pin GPIO_PIN_15
#define LED_BLUE_GPIO_Port GPIOC
#define MOTOR1_AN2_Pin GPIO_PIN_0
#define MOTOR1_AN2_GPIO_Port GPIOC
#define MOTOR1_AN1_Pin GPIO_PIN_1
#define MOTOR1_AN1_GPIO_Port GPIOC
#define MOTOR2_AN1_Pin GPIO_PIN_2
#define MOTOR2_AN1_GPIO_Port GPIOC
#define MOTOR2_AN2_Pin GPIO_PIN_3
#define MOTOR2_AN2_GPIO_Port GPIOC
#define MOTOR1_Pin GPIO_PIN_0
#define MOTOR1_GPIO_Port GPIOA
#define MOTOR2_Pin GPIO_PIN_1
#define MOTOR2_GPIO_Port GPIOA
#define MOTOR_STBY_Pin GPIO_PIN_4
#define MOTOR_STBY_GPIO_Port GPIOA
#define ENCODER2_A_Pin GPIO_PIN_6
#define ENCODER2_A_GPIO_Port GPIOA
#define ENCODER2_B_Pin GPIO_PIN_7
#define ENCODER2_B_GPIO_Port GPIOA
#define KEY_1_Pin GPIO_PIN_4
#define KEY_1_GPIO_Port GPIOC
#define VOLTAGE_Pin GPIO_PIN_0
#define VOLTAGE_GPIO_Port GPIOB
#define CURRENT_Pin GPIO_PIN_1
#define CURRENT_GPIO_Port GPIOB
#define BEEP_Pin GPIO_PIN_2
#define BEEP_GPIO_Port GPIOB
#define PS2_DI_Pin GPIO_PIN_12
#define PS2_DI_GPIO_Port GPIOB
#define PS2_DO_Pin GPIO_PIN_13
#define PS2_DO_GPIO_Port GPIOB
#define PS2_CS_Pin GPIO_PIN_14
#define PS2_CS_GPIO_Port GPIOB
#define PS2_CLK_Pin GPIO_PIN_15
#define PS2_CLK_GPIO_Port GPIOB
#define MPU_IIC_SCL_Pin GPIO_PIN_8
#define MPU_IIC_SCL_GPIO_Port GPIOC
#define MPU_IIC_SDA_Pin GPIO_PIN_9
#define MPU_IIC_SDA_GPIO_Port GPIOC
#define SERVO3_Pin GPIO_PIN_15
#define SERVO3_GPIO_Port GPIOA
#define MPU_INT_Pin GPIO_PIN_10
#define MPU_INT_GPIO_Port GPIOC
#define MPU_INT_EXTI_IRQn EXTI15_10_IRQn
#define SWITCH2_Pin GPIO_PIN_11
#define SWITCH2_GPIO_Port GPIOC
#define SWITCH1_Pin GPIO_PIN_12
#define SWITCH1_GPIO_Port GPIOC
#define SERVO2_Pin GPIO_PIN_3
#define SERVO2_GPIO_Port GPIOB
#define ENCODER1_A_Pin GPIO_PIN_6
#define ENCODER1_A_GPIO_Port GPIOB
#define ENCODER1_B_Pin GPIO_PIN_7
#define ENCODER1_B_GPIO_Port GPIOB
#define SERVO1_Pin GPIO_PIN_9
#define SERVO1_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
