//
// Created by Kaijun on 2020/9/11.
//
#ifndef __OLED_H
#define __OLED_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"

#define u32 uint32_t
#define u8 uint8_t
//-----------------OLED端口定义----------------
#define OLED_RST_Clr() HAL_GPIO_WritePin(GPIOE,GPIO_PIN_7,GPIO_PIN_RESET) //PEout(7)=0   //RST
#define OLED_RST_Set() HAL_GPIO_WritePin(GPIOE,GPIO_PIN_7,GPIO_PIN_SET) //PEout(7)=1   //RST

#define OLED_RS_Clr() HAL_GPIO_WritePin(GPIOE,GPIO_PIN_8,GPIO_PIN_RESET) // PEout(8)=0    //DC
#define OLED_RS_Set() HAL_GPIO_WritePin(GPIOE,GPIO_PIN_8,GPIO_PIN_SET) //PEout(8)=1    //DC

#define OLED_SCLK_Clr() HAL_GPIO_WritePin(GPIOE,GPIO_PIN_5,GPIO_PIN_RESET) // PEout(5)=0  //SCL
#define OLED_SCLK_Set() HAL_GPIO_WritePin(GPIOE,GPIO_PIN_5,GPIO_PIN_SET) // PEout(5)=1   //SCL

#define OLED_SDIN_Clr() HAL_GPIO_WritePin(GPIOE,GPIO_PIN_6,GPIO_PIN_RESET) // PEout(6)=0   //SDA
#define OLED_SDIN_Set() HAL_GPIO_WritePin(GPIOE,GPIO_PIN_6,GPIO_PIN_SET) // PEout(6)=1   //SDA

#define OLED_CMD  0    //写命令
#define OLED_DATA 1    //写数据

//OLED控制用函数
void OLED_WR_Byte(u8 dat, u8 cmd);

void OLED_Display_On(void);

void OLED_Display_Off(void);

void OLED_Refresh_Gram(void);

void OLED_Init(void);

void OLED_Clear(void);

void OLED_DrawPoint(u8 x, u8 y, u8 t);

void OLED_ShowChar(u8 x, u8 y, u8 chr, u8 size, u8 mode);

void OLED_ShowNumber(u8 x, u8 y, u32 num, u8 len, u8 size);

void OLED_ShowString(u8 x, u8 y, const u8 *p);

void OLED_ShowChinese(u8 x,u8 y,u8 num,u8 size1);


#ifdef __cplusplus
}
#endif
#endif