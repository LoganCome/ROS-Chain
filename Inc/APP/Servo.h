#ifndef _APP_SERVO_H_
#define _APP_SERVO_H_
#include "BSP/bsp_servo.h"
#include <stdbool.h>
typedef struct{

	float fCurrentAngle; 
	float fTargetAngle;
	uint16_t usPwm;
	double fPrevError;
	double fIntegral;
	double fDerivative;
	float fOutPutAngle;
	float fVelocity;
}Servo_t;

void vServoStart(Servo_t* pxServo);
void vSetAngleOffset(int16_t sPwm);
void vSetServoAngle(Servo_t* pxServo, bool bPid);
	
#endif
