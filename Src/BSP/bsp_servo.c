#include "BSP/bsp_servo.h"
#include <stdbool.h>
#define FULL_PWM		20000

#define SERVO_UP_PWM 1870
#define SERVO_DOWN_PWM 1160

uint16_t usServoPwm = SERVO_INIT;
int16_t sServoPwmOffset = 0;
bool bServoFlag = false;
extern TIM_HandleTypeDef htim1;

/**
call in HAL_TIM_PeriodElapsedCallback function
*/
void vServoCallback(TIM_HandleTypeDef *htim){
	if(htim->Instance == SERVO_TIMER){
		if(bServoFlag == true){
			HAL_GPIO_WritePin(SERVO1_GPIO_Port,SERVO1_Pin,GPIO_PIN_SET);
			HAL_GPIO_WritePin(SERVO2_GPIO_Port,SERVO2_Pin,GPIO_PIN_SET);
			HAL_GPIO_WritePin(SERVO3_GPIO_Port,SERVO3_Pin,GPIO_PIN_SET);			
			SERVO_TIMER->ARR = usServoPwm;
			bServoFlag = false;
		}
		else{
			HAL_GPIO_WritePin(SERVO1_GPIO_Port,SERVO1_Pin,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(SERVO2_GPIO_Port,SERVO2_Pin,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(SERVO3_GPIO_Port,SERVO3_Pin,GPIO_PIN_RESET);
			SERVO_TIMER->ARR = FULL_PWM - usServoPwm;
			bServoFlag = true;
		}	
	}
}

void TIM6_IRQHandlerCallback(TIM_HandleTypeDef *htim){
	vServoCallback(htim);
}

void vSetServoPwm(uint16_t usPwm){
	(usPwm > SERVO_UP_PWM)?(usPwm = SERVO_UP_PWM):((usPwm < SERVO_DOWN_PWM)?(usPwm = SERVO_DOWN_PWM):usPwm);
	usServoPwm = usPwm;
}

void vSetServoPwmOffset(int16_t sPwm){
	sServoPwmOffset = sPwm;
}

void vServoInit(void){
	HAL_TIM_Base_Start_IT(&htim1);
}
