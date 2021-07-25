#include "APP/Kinematics.h"
#include "APP/Motor.h"
#include "APP/config.h"
#if BASE_TYPE == BASE_ACKERMAN
#include "APP/servo.h"
#endif

#include <stdbool.h>

#include "APP/IMU.h"


#define PI		3.14159

#define MAX_TURN_ANGLE 0.6
#define MIN_TURN_ANGLE -0.6

/*
MOTOR Position Define:
		 FORWORD
	M3				M4
			CAR
	M1				M2
		BACKWORD
*/
extern	Motor_t xMotor1;
extern	Motor_t xMotor2;
extern float fGyroZ;
#if BASE_TYPE == BASE_4WD
	extern	Motor_t xMotor3;
	extern	Motor_t xMotor4;
#endif
#if BASE_TYPE == BASE_ACKERMAN
	extern	Servo_t xServo;
#endif

float fBaseWidth = BASE_WIDTH;
bool bMecanum = false;
#if BASE_TYPE == BASE_4WD
float fBaseLength = BASE_LENGTH;
#endif

float fWheelDiameter = WHEEL_DIAMETER0;

Velocity_t xVelocity;
Velocity_t xTargetVelocity;
Odom_t xOdom;


void vSetKinematicsConfig(uint8_t param){
	switch(param){
//normal wheel type
#ifdef WHEEL_DIAMETER1
		case 0:
			fWheelDiameter = WHEEL_DIAMETER0;
		break;
#endif
#ifdef WHEEL_DIAMETER1
		case 1:
			fWheelDiameter = WHEEL_DIAMETER1;
		break;
#endif
#ifdef WHEEL_DIAMETER2
		case 2:
			fWheelDiameter = WHEEL_DIAMETER2;
		break;
#endif
#ifdef WHEEL_DIAMETER3
		case 3:
			fWheelDiameter = WHEEL_DIAMETER3;
		break;
#endif
//Mecanum wheel type
#ifdef MECANUM_WHEEL_DIAMETER0
		case 0+4:
			fWheelDiameter = MECANUM_WHEEL_DIAMETER0;
			
		break;
#endif
#ifdef MECANUM_WHEEL_DIAMETER1
		case 1+4:
			fWheelDiameter = MECANUM_WHEEL_DIAMETER1;
		break;
#endif
#ifdef MECANUM_WHEEL_DIAMETER2
		case 2+4:
			fWheelDiameter = MECANUM_WHEEL_DIAMETER2;
			fBaseWidth = 0.255;
		break;
#endif
#ifdef MECANUM_WHEEL_DIAMETER3
		case 3+4:
			fWheelDiameter = MECANUM_WHEEL_DIAMETER3;
		break;
#endif
		default:
			break;
	}
}
/**

*/
uint16_t usConvertAcc2MotorAcc(float fAccel){
	return (uint16_t)(((fAccel*60)/(1000/MOTOR_CONTROL_PERIOD))/(fWheelDiameter*3.14));
}
/**

*/
void vComputeMotorSpeed(float fLineX,float fLineY,float fAngle){
	int16_t sLeftRpm,sRightRpm; 
	static float fLineXMinute,fTanVel;  //convert m/s to m/min
	#if BASE_TYPE != BASE_ACKERMAN	
		float fAngleMinute;
	#endif
	xTargetVelocity.fLineX = fLineX;
	xTargetVelocity.fAngleZ = fAngle;
	#if BASE_TYPE == BASE_ACKERMAN
		fAngle = atanf(fAngle*WHEEL_BASE/fLineX);//convert Omega to steering angle  
		if(fAngle > 0.5) fAngle = 0.5;
		if(fAngle < -0.5) fAngle = -0.5;
	#endif	
	fLineXMinute = fLineX*60;
	#if BASE_TYPE != BASE_ACKERMAN	
		fAngleMinute = fAngle*60;
	#endif
		
	#if BASE_TYPE == BASE_4WD
		float fLineYMinute;
		fLineYMinute = fLineY*60;
	#endif
	if(bMecanum){
#if BASE_TYPE == BASE_4WD
		xMotor1.sTargetRpm = (fLineXMinute - fLineYMinute - fAngleMinute*(fBaseLength + fBaseWidth)/2) / (PI*fWheelDiameter);
		xMotor2.sTargetRpm = (fLineXMinute + fLineYMinute + fAngleMinute*(fBaseLength + fBaseWidth)/2) / (PI*fWheelDiameter);
		xMotor3.sTargetRpm = (fLineXMinute + fLineYMinute - fAngleMinute*(fBaseLength + fBaseWidth)/2) / (PI*fWheelDiameter);
		xMotor4.sTargetRpm = (fLineXMinute - fLineYMinute + fAngleMinute*(fBaseLength + fBaseWidth)/2) / (PI*fWheelDiameter);
#endif
	}
	else{
	#if BASE_TYPE == BASE_2WD
		fTanVel = fAngleMinute * (fBaseWidth/2);
	#elif	BASE_TYPE == BASE_4WD
		fTanVel = fAngleMinute * (fBaseWidth/2) * (1/cos(atanf(fBaseLength/fBaseWidth))) * (1/cos(atan(fBaseLength/fBaseWidth)));
	#elif BASE_TYPE == BASE_ACKERMAN
		fTanVel = fAngle;
	#endif

	#if BASE_TYPE == BASE_ACKERMAN
		sLeftRpm = fLineXMinute/(PI*fWheelDiameter)*(1-fBaseWidth*tan(fAngle)/2/WHEEL_BASE);
		sRightRpm = fLineXMinute/(PI*fWheelDiameter)*(1+fBaseWidth*tan(fAngle)/2/WHEEL_BASE);
	#else
		sLeftRpm = fLineXMinute/(PI*fWheelDiameter) - fTanVel/(PI*fWheelDiameter);
		sRightRpm = fLineXMinute/(PI*fWheelDiameter) + fTanVel/(PI*fWheelDiameter);
	#endif
		if(sLeftRpm > usMaxRpm)
			sLeftRpm = usMaxRpm;
		else if(sLeftRpm < -usMaxRpm)
			sLeftRpm = -usMaxRpm;

		if(sRightRpm > usMaxRpm)
			sRightRpm = usMaxRpm;
		else if(sRightRpm < -usMaxRpm)
			sRightRpm = -usMaxRpm;	
		
		xMotor1.sTargetRpm = sLeftRpm;
		xMotor2.sTargetRpm = sRightRpm;
	#if BASE_TYPE == BASE_4WD
		xMotor3.sTargetRpm = sLeftRpm;	
		xMotor4.sTargetRpm = sRightRpm;	
	#endif 	

	#if BASE_TYPE == BASE_ACKERMAN
//		vSetServoPwm(usServoCurrentPwm);
		xServo.fTargetAngle = fTanVel;
	#endif	
	}	
}
/**

*/
void vComputeCarMotorSpeed(float fLineX,float fAccelX,float fAngle){
	#if BASE_TYPE == BASE_ACKERMAN
	int16_t sLeftRpm,sRightRpm; 
	static float fLineXMinute;  //convert m/s to m/min
	
	xTargetVelocity.fLineX = fLineX;
	xTargetVelocity.fAngleZ = fAngle;
	
//	if(fAngle > 0.5) fAngle = 0.5;
//	if(fAngle < -0.5) fAngle = -0.5;

	(fAngle > MAX_TURN_ANGLE)?(fAngle = MAX_TURN_ANGLE):((fAngle < MIN_TURN_ANGLE)?(fAngle = MIN_TURN_ANGLE):fAngle);
	
	fLineXMinute = fLineX*60;
	sLeftRpm = fLineXMinute/(PI*fWheelDiameter)*(1-fBaseWidth*tan(xServo.fTargetAngle)/2/WHEEL_BASE);
	sRightRpm = fLineXMinute/(PI*fWheelDiameter)*(1+fBaseWidth*tan(xServo.fTargetAngle)/2/WHEEL_BASE);
	//motor speed threshold
	if(sLeftRpm > usMaxRpm)
		sLeftRpm = usMaxRpm;
	else if(sLeftRpm < -usMaxRpm)
		sLeftRpm = -usMaxRpm;

	if(sRightRpm > usMaxRpm)
		sRightRpm = usMaxRpm;
	else if(sRightRpm < -usMaxRpm)
		sRightRpm = -usMaxRpm;	
	//set value to motor
	xMotor1.sTargetRpm = sLeftRpm;
	xMotor2.sTargetRpm = sRightRpm;
	//set servo angle
	xServo.fTargetAngle = fAngle;
//	xServo.fCurrentAngle = atanf(xImu.pfGyro[2]*L/(xVelocity.fLineX));	//calculate current real angle
//	CONVERT_OMEGA2ANGLE(xImu.pfGyro[2],xVelocity.fLineX);
	#endif
}
/**

*/
void vGetSpeed(Velocity_t* xVelocity){
	double fAverageRpsX,fAverageRpsY;
	double fAverageRpsA; 
#if BASE_TYPE == BASE_4WD
	fAverageRpsX  = (xMotor1.sCurrentRpm + xMotor2.sCurrentRpm + xMotor3.sCurrentRpm + xMotor4.sCurrentRpm)/4;
	fAverageRpsY  = (xMotor1.sCurrentRpm + xMotor4.sCurrentRpm - xMotor3.sCurrentRpm - xMotor2.sCurrentRpm)/4;
	fAverageRpsA = ((xMotor2.sCurrentRpm + xMotor4.sCurrentRpm) - xMotor1.sCurrentRpm - xMotor3.sCurrentRpm)/2.0;
#elif BASE_TYPE == BASE_2WD||BASE_TYPE == BASE_ACKERMAN 
	fAverageRpsX  = (xMotor1.sCurrentRpm + xMotor2.sCurrentRpm)/2;
	fAverageRpsA = (xMotor2.sCurrentRpm -xMotor1.sCurrentRpm);	
#endif
	fAverageRpsX /= 60.0;
	xVelocity->fLineX = fAverageRpsX*(fWheelDiameter*PI);
	fAverageRpsY /= 60.0;	
	xVelocity->fLineY = fAverageRpsY*(fWheelDiameter*PI);
	fAverageRpsA /= 60.0;
#if BASE_TYPE == BASE_4WD
	if(bMecanum == true){
//		xVelocity->fAngleZ = fAverageRpsA*(fWheelDiameter*PI)/(2*(fBaseLength+fBaseWidth));
			xVelocity->fAngleZ = fAverageRpsA*(fWheelDiameter*PI)/(1/cos(atanf(fBaseLength/fBaseWidth)))/(fBaseWidth)/(1/cos(atanf(fBaseLength/fBaseWidth)));
	}
	else{
		xVelocity->fAngleZ = fAverageRpsA*(fWheelDiameter*PI)/(1/cos(atanf(fBaseLength/fBaseWidth)))/(fBaseWidth)/(1/cos(atanf(fBaseLength/fBaseWidth)));
	}
#elif BASE_TYPE == BASE_2WD
	xVelocity->fAngleZ = fAverageRpsA*(fWheelDiameter*PI)/fBaseWidth;
#elif BASE_TYPE == BASE_ACKERMAN 
//	xVelocity->fAngleZ = xImu.pfGyro[2];
	xVelocity->fAngleZ = fGyroZ;
#endif
}

