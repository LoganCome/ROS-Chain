#ifndef __BSP_BSP_SERVO_H_
#define __BSP_BSP_SERVO_H_
#include "main.h"

#define SERVO_TIMER	TIM1
#define SERVO_INIT 1500

extern int16_t sServoPwm;
extern int16_t sServoPwmOffset;
void vServoCallback(TIM_HandleTypeDef *htim);
void vServoRun(void);
void vSetServoPwm(uint16_t usPwm);
void vSetServoPwmOffset(int16_t sPwm);
void vServoInit(void);
#endif
