#ifndef __BSP_BSP_MOTOR_H_
#define __BSP_BSP_MOTOR_H_
#include "main.h"
typedef enum{
	MOTOR1 = 0,
	MOTOR2,	
	MOTOR3,
	MOTOR4
}motor_t;

typedef enum{
	MOTOR_CW,
	MOTOR_CCW
}motor_dir_t;
void vSetMotorPWM(motor_t motor,int16_t pwm);
#endif
