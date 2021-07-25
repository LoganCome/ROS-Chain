#ifndef __BSP_OLED_I2C_H__
#define	__BSP_OLED_I2C_H__
/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* 类型定义 ------------------------------------------------------------------*/
/* 宏定义 --------------------------------------------------------------------*/

#define OLED_I2C_WR	        0		/* 写控制bit */
#define OLED_I2C_RD	        1		/* 读控制bit */

#define OLED_I2C_GPIO_CLK_ENABLE()               __HAL_RCC_GPIOB_CLK_ENABLE()
#define OLED_I2C_GPIO_PORT                       GPIOB   
#define OLED_I2C_SCL_PIN                         GPIO_PIN_11
#define OLED_I2C_SDA_PIN                         GPIO_PIN_10

#define OLED_I2C_SCL_HIGH()                      HAL_GPIO_WritePin(OLED_I2C_GPIO_PORT,OLED_I2C_SCL_PIN,GPIO_PIN_SET)    // 输出高电平
#define OLED_I2C_SCL_LOW()                       HAL_GPIO_WritePin(OLED_I2C_GPIO_PORT,OLED_I2C_SCL_PIN,GPIO_PIN_RESET)  // 输出低电平
#define OLED_I2C_SDA_HIGH()                      HAL_GPIO_WritePin(OLED_I2C_GPIO_PORT,OLED_I2C_SDA_PIN,GPIO_PIN_SET)    // 输出高电平
#define OLED_I2C_SDA_LOW()                       HAL_GPIO_WritePin(OLED_I2C_GPIO_PORT,OLED_I2C_SDA_PIN,GPIO_PIN_RESET)  // 输出低电平
#define OLED_I2C_SDA_READ()                      HAL_GPIO_ReadPin(OLED_I2C_GPIO_PORT,OLED_I2C_SDA_PIN)



/* 扩展变量 ------------------------------------------------------------------*/
/* 函数声明 ------------------------------------------------------------------*/
void    OLED_I2C_Start(void);
void    OLED_I2C_Stop(void);
void    OLED_I2C_SendByte(uint8_t _ucByte);
uint8_t OLED_I2C_ReadByte(void);
uint8_t OLED_I2C_WaitAck(void);
void    OLED_I2C_Ack(void);
void    OLED_I2C_NAck(void);
void OLED_I2C_InitGPIO(void);
uint8_t read_OLED_I2C(uint8_t device,uint8_t addr);
void write_OLED_I2C(uint8_t device,uint8_t addr,uint8_t dat);


#endif /* __BSP_OLED_I2C_H__ */

/******************* (C) COPYRIGHT 2015-2020 硬石嵌入式开发团队 *****END OF FILE****/
