#ifndef __BSP_OLED_I2C_H__
#define	__BSP_OLED_I2C_H__
/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* ���Ͷ��� ------------------------------------------------------------------*/
/* �궨�� --------------------------------------------------------------------*/

#define OLED_I2C_WR	        0		/* д����bit */
#define OLED_I2C_RD	        1		/* ������bit */

#define OLED_I2C_GPIO_CLK_ENABLE()               __HAL_RCC_GPIOB_CLK_ENABLE()
#define OLED_I2C_GPIO_PORT                       GPIOB   
#define OLED_I2C_SCL_PIN                         GPIO_PIN_11
#define OLED_I2C_SDA_PIN                         GPIO_PIN_10

#define OLED_I2C_SCL_HIGH()                      HAL_GPIO_WritePin(OLED_I2C_GPIO_PORT,OLED_I2C_SCL_PIN,GPIO_PIN_SET)    // ����ߵ�ƽ
#define OLED_I2C_SCL_LOW()                       HAL_GPIO_WritePin(OLED_I2C_GPIO_PORT,OLED_I2C_SCL_PIN,GPIO_PIN_RESET)  // ����͵�ƽ
#define OLED_I2C_SDA_HIGH()                      HAL_GPIO_WritePin(OLED_I2C_GPIO_PORT,OLED_I2C_SDA_PIN,GPIO_PIN_SET)    // ����ߵ�ƽ
#define OLED_I2C_SDA_LOW()                       HAL_GPIO_WritePin(OLED_I2C_GPIO_PORT,OLED_I2C_SDA_PIN,GPIO_PIN_RESET)  // ����͵�ƽ
#define OLED_I2C_SDA_READ()                      HAL_GPIO_ReadPin(OLED_I2C_GPIO_PORT,OLED_I2C_SDA_PIN)



/* ��չ���� ------------------------------------------------------------------*/
/* �������� ------------------------------------------------------------------*/
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

/******************* (C) COPYRIGHT 2015-2020 ӲʯǶ��ʽ�����Ŷ� *****END OF FILE****/
