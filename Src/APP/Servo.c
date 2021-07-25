#include "APP/servo.h"
#include "APP/Kinematics.h"

#define K 700.0f
extern Velocity_t xTargetVelocity;


void vServoStart(Servo_t* pxServo){
	vServoInit();
	pxServo->fTargetAngle = 0;
	pxServo->fIntegral = 0.0;
	pxServo->fPrevError = 0.0;
	pxServo->fDerivative = 0.0;
}

void vSetAngleOffset(int16_t sPwm){
	vSetServoPwmOffset(sPwm);
}
void vSetServoAngle(Servo_t* pxServo, bool bPid){
	double fError;
	float fAngleToServo = 0;
	if (pxServo->fTargetAngle > 0.0)
		fAngleToServo = K;
	else
		fAngleToServo = K*1.25;
	if (!bPid){
		pxServo->usPwm= SERVO_INIT + (pxServo->fTargetAngle)*fAngleToServo - sServoPwmOffset;
	}
	else{
			float fOutPutAngle = 0.0;
			if((pxServo->fVelocity < 0.01)&&(pxServo->fVelocity > -0.01)){
//			if((xTargetVelocity.fLineX < 0.01)&&(xTargetVelocity.fLineX > -0.01)){				
				fOutPutAngle = pxServo->fTargetAngle;
				pxServo->fIntegral = 0.0;
				pxServo->fPrevError = 0.0;
				pxServo->fDerivative = 0.0;
			}
			else{
				fError = pxServo->fTargetAngle - pxServo->fCurrentAngle;
				if(fabs(fError)>0.02){
					pxServo->fIntegral += fError;
					pxServo->fDerivative = fError - pxServo->fPrevError;
					if(pxServo->fIntegral > 1)		// Integral threshold 
						pxServo->fIntegral = 1;
					else if (pxServo->fIntegral < -1)
						pxServo->fIntegral = -1;
					else;
				}
				fOutPutAngle = pxServo->fTargetAngle + fError*0.2 + pxServo->fIntegral*0.1 + pxServo->fDerivative*0.0;
				
				pxServo->fPrevError = fError;

			}
			pxServo->fOutPutAngle = fOutPutAngle;
//			printf("Angle %f\r\n",fOutPutAngle);
			pxServo-> usPwm = SERVO_INIT - (fOutPutAngle)*fAngleToServo - sServoPwmOffset;
	}
	vSetServoPwm(pxServo->usPwm);
}
