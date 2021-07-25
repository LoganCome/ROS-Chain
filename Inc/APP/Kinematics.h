#ifndef __APP_KINEMATICS_H_
#define __APP_KINEMATICS_H_
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

//#define T 0.156f
#define WHEEL_BASE 0.1445

#define CONVERT_OMEGA2ANGLE(Omega,v)  atanf((float)(Omega*WHEEL_BASE/v))

typedef struct{
	float fLineX;
	float fLineY;
	float fAngleZ;
}Velocity_t;

typedef struct{
	float x;
	float y;
}Position_t;

typedef struct{
	Position_t xPos;
}Odom_t;

extern Velocity_t xVelocity;
extern Odom_t xOdom;
extern float fWheelDiameter;
extern bool bMecanum;

void vSetKinematicsConfig(uint8_t param);
uint16_t usConvertAcc2MotorAcc(float fAccel);
void vComputeMotorSpeed(float fLineX,float fLineY,float fAngle);
void vComputeCarMotorSpeed(float fLineX,float fAccelX,float fAngle);
void vGetSpeed(Velocity_t* xVelocity);
#endif
