#include "BSP/bsp_motor.h"
#include <stdlib.h>

GPIO_TypeDef*  MOTOR_AN1_PORT[] = {MOTOR1_AN1_GPIO_Port,MOTOR2_AN1_GPIO_Port};
GPIO_TypeDef*  MOTOR_AN2_PORT[] = {MOTOR1_AN2_GPIO_Port,MOTOR2_AN2_GPIO_Port};
const uint16_t	MOTOR_AN2_PIN[] = {MOTOR1_AN2_Pin,MOTOR2_AN2_Pin};
const uint16_t	MOTOR_AN1_PIN[] = {MOTOR1_AN1_Pin,MOTOR2_AN1_Pin};

TIM_TypeDef*  MOTOR_TIMER[] = {TIM2,TIM2,TIM2,TIM2};


const uint32_t MOTOR_TIMER_CHANNEL[] = {TIM_CHANNEL_1,TIM_CHANNEL_2,TIM_CHANNEL_3,TIM_CHANNEL_4};
/**
set motor run direction
CW and CCW is defined as from motor output shaft,shaft run direction
xMotor:motor ID
xDir:motor run direction
*/
static void vSetMotorDir(motor_t xMotor,motor_dir_t xDir){
	if(xDir == MOTOR_CCW){
		HAL_GPIO_WritePin(MOTOR_AN1_PORT[xMotor],MOTOR_AN1_PIN[xMotor],GPIO_PIN_SET);
		HAL_GPIO_WritePin(MOTOR_AN2_PORT[xMotor],MOTOR_AN2_PIN[xMotor],GPIO_PIN_RESET);
	}
	else if(xDir == MOTOR_CW){
		HAL_GPIO_WritePin(MOTOR_AN1_PORT[xMotor],MOTOR_AN1_PIN[xMotor],GPIO_PIN_RESET);
		HAL_GPIO_WritePin(MOTOR_AN2_PORT[xMotor],MOTOR_AN2_PIN[xMotor],GPIO_PIN_SET);	
	}
}
/**
set motor pwm
xMotor:motor ID
sPwm:>0 is CW direction,<0 is CCW direction
*/
void vSetMotorPWM(motor_t xMotor,int16_t sPwm){
	uint16_t usPwm = 0;
	if(sPwm > 0){
		vSetMotorDir(xMotor,MOTOR_CW);
	}
	else if(sPwm < 0){
		vSetMotorDir(xMotor,MOTOR_CCW);
	}
	else{	/*TODO motor break*/
	
	}
	usPwm = abs(sPwm);
	*(__IO uint32_t *)(&(MOTOR_TIMER[xMotor]->CCR1) + ((MOTOR_TIMER_CHANNEL[xMotor]) >> 2U)) = (usPwm);	
}
