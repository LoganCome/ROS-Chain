#ifndef __BSP_SIMI2C_H__
#define	__BSP_SIMI2C_H__

#include "stm32f1xx_hal.h"

#define I2C_WR	               0		/* write control bit */
#define I2C_RD	               1		/* read control  bit */

#define I2C_GPIO_CLK_ENABLE()               __HAL_RCC_GPIOA_CLK_ENABLE()
#define I2C_GPIO_PORT                       GPIOC  
#define I2C_SCL_PIN                         GPIO_PIN_8
#define I2C_SDA_PIN                         GPIO_PIN_9

#define I2C_SCL_HIGH()                      HAL_GPIO_WritePin(I2C_GPIO_PORT,I2C_SCL_PIN,GPIO_PIN_SET)    
#define I2C_SCL_LOW()                       HAL_GPIO_WritePin(I2C_GPIO_PORT,I2C_SCL_PIN,GPIO_PIN_RESET)  
#define I2C_SDA_HIGH()                      HAL_GPIO_WritePin(I2C_GPIO_PORT,I2C_SDA_PIN,GPIO_PIN_SET)    
#define I2C_SDA_LOW()                       HAL_GPIO_WritePin(I2C_GPIO_PORT,I2C_SDA_PIN,GPIO_PIN_RESET)  
#define I2C_SDA_READ()                      HAL_GPIO_ReadPin(I2C_GPIO_PORT,I2C_SDA_PIN)

void    I2C_Start(void);
void    I2C_Stop(void);
void    I2C_SendByte(uint8_t _ucByte);
uint8_t I2C_ReadByte(uint8_t ack);
uint8_t I2C_WaitAck(void);
void    I2C_Ack(void);
void    I2C_NAck(void);
uint8_t I2C_CheckDevice(uint8_t _Address);
void I2C_InitGPIO(void);



#endif

