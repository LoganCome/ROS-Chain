#include "APP/Motor.h"
#include "APP/config.h"
PID_t xPid;

uint16_t usMotorMaxRpm = 10000;
uint16_t usRatio = RATIO0;
uint16_t usPulsePerRound = ENCODER*RATIO0*4;
uint16_t usMaxRpm = 10000/(RATIO0+1);
uint16_t usAccelDecel = 400;

extern uint8_t pucHardwareVersion[3];

void vSetMotorConfig(uint8_t param){
	switch(param){
		case 0:
			usRatio = RATIO0;
			break;
#ifdef RATIO1
		case 1:
			usRatio = RATIO1;
			break;
#endif
		default:
			break;
	}
	usPulsePerRound = ENCODER*usRatio*4;;
	usMaxRpm = usMotorMaxRpm/(usRatio+1);
}
void vSetMotorAccelDecel(uint16_t usSetAccelDecel){
	usAccelDecel = usSetAccelDecel;
}
/**

*/
int16_t sRpm2PWM(double sRpm){
	double fTemp = 0;
	//remap scale of target RPM vs usMaxRpm to PWM
	if(sRpm > usMaxRpm) sRpm = usMaxRpm;
	if(sRpm < -usMaxRpm) sRpm = -usMaxRpm;
	fTemp = (double)sRpm / (double)175.00;
	fTemp *= 1000;
	return (int16_t)fTemp;
}

void vPidInit(void){
	xPid.p = K_P;
	xPid.i = K_I;
	xPid.d = K_D;
}

void vMotorInit(Motor_t *pxMotor,	motor_t xMotor, encoder_t xEncoder){
	pxMotor->xEncoder = xEncoder;
	pxMotor->xMotor = xMotor;
	pxMotor->usEncoderValuePerRound = usPulsePerRound;
	pxMotor->iLastEncoderReadValue = 0;
	pxMotor->uiLastEncoderReadTime = 0;
}

void vGetMotorSpeed(Motor_t *pxMotor){
	int32_t iEncoderDelta = 0;
	uint32_t uiTickGap = 0;
	iEncoderDelta = iGetEncoderValue(pxMotor->xEncoder) - pxMotor->iLastEncoderReadValue;
	pxMotor->iLastEncoderReadValue = iGetEncoderValue(pxMotor->xEncoder);
	uiTickGap = HAL_GetTick() - pxMotor->uiLastEncoderReadTime;
	pxMotor->uiLastEncoderReadTime = HAL_GetTick();
	#ifdef LINEAR_ACTUATOR
	pxMotor->sCurrentRpm = (95*iEncoderDelta)/(int32_t)(uiTickGap);
	#else
	pxMotor->sCurrentRpm = (60000*iEncoderDelta)/(int32_t)(uiTickGap*(pxMotor->usEncoderValuePerRound));
	#endif
}

void vSetMotorSpeed(Motor_t *pxMotor,int16_t sRpm){
	pxMotor->sTargetRpm = sRpm;
}

void vExecuteMotorSpeed(Motor_t *pxMotor,double sRpm){
	if((pucHardwareVersion[1] == 2) || (pucHardwareVersion[1] == 3) ){
		vSetMotorPWM(pxMotor->xMotor,-sRpm2PWM(sRpm));
	}
	else{
		vSetMotorPWM(pxMotor->xMotor,sRpm2PWM(sRpm));
	}
//	if(pucHardwareVersion[1] == 2){
//		vSetMotorPWM(pxMotor->xMotor,sRpm2PWM(sRpm));
//	}
//	else{
//		vSetMotorPWM(pxMotor->xMotor,-sRpm2PWM(sRpm));
//	}
}

double fMotorPidCompute(Motor_t *pxMotor){
	double fError;
	double fPidResult;

	if(pxMotor->sTargetRpm > pxMotor->sSmoothTargetRpm){
		pxMotor->sSmoothTargetRpm += usAccelDecel;
		if(pxMotor->sSmoothTargetRpm > pxMotor->sTargetRpm)
			pxMotor->sSmoothTargetRpm = pxMotor->sTargetRpm;	
	}
	else if(pxMotor->sTargetRpm < pxMotor->sSmoothTargetRpm){
		pxMotor->sSmoothTargetRpm -= usAccelDecel;
		if(pxMotor->sSmoothTargetRpm < pxMotor->sTargetRpm)
			pxMotor->sSmoothTargetRpm = pxMotor->sTargetRpm;	
	}

	fError = pxMotor->sSmoothTargetRpm - pxMotor->sCurrentRpm;
	pxMotor->fIntegral += fError;
	pxMotor->fDerivative = fError - pxMotor->fPrevError;
	pxMotor->fPrevError = fError;
	if(pxMotor->sSmoothTargetRpm == 0.0 && fError == 0.0){
		pxMotor->fIntegral = 0;
	}
	if(pxMotor->fIntegral > 2000)
		pxMotor->fIntegral = 2000;
	else if (pxMotor->fIntegral < -2000)
		pxMotor->fIntegral = -2000;
	
	fPidResult = fError*xPid.p + pxMotor->fIntegral*xPid.i + pxMotor->fDerivative*xPid.d;
	pxMotor->sSetRpm = (uint16_t)fPidResult;
	return fPidResult;
}



void vUpdataMotorPwm(Motor_t *pxMotor,int16_t sPwm){

}
