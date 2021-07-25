#ifndef __APP_MOTOR_H_
#define __APP_MOTOR_H_
#include "BSP/bsp_encoder.h"
#include "BSP/bsp_motor.h"

typedef struct{
	uint16_t usMaxRpm;
	uint16_t usEncoderValuePerRound;
	uint32_t uiLastEncoderReadTime;
	int32_t iLastEncoderReadValue;
	encoder_t xEncoder;
	motor_t xMotor;
	int16_t sCurrentRpm; 
	int16_t sSmoothTargetRpm;
	int16_t sTargetRpm;
	int16_t sSetRpm;
	double fPrevError;
	double fIntegral;
	double fDerivative;
	double fMaxError;
}Motor_t;

typedef struct{
	float p;
	float i;
	float d;
}PID_t;

extern uint16_t usMotorMaxRpm;
extern uint16_t usRatio;
extern uint16_t usPulsePerRound;
extern uint16_t usMaxRpm;

void vSetMotorAccelDecel(uint16_t usSetAccelDecel);
void vSetMotorConfig(uint8_t param);
void vPidInit(void);
void vMotorInit(Motor_t *pxMotor,	motor_t xMotor, encoder_t xEncoder);
int16_t sRpm2PWM(double sRpm);
void vGetMotorSpeed(Motor_t *pxMotor);
void vSetMotorSpeed(Motor_t *pxMotor,int16_t sRpm);
void vExecuteMotorSpeed(Motor_t *pxMotor,double sRpm);
double fMotorPidCompute(Motor_t *pxMotor);
#endif


