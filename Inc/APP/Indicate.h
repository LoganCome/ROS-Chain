#ifndef __APP_INDICATE_H_
#define __APP_INDICATE_H_
#include "main.h"

typedef enum{
	LEDR,
	LEDG,	
	LEDB,
	BEEP,
	DEVICE_NUM
}IndicateDeviceName_t;

typedef enum{
	INDICATE_DEVICE_OFF,
	INDICATE_DEVICE_ON
}IndicateDeviceState_t;

typedef struct{
	IndicateDeviceName_t xDevice;
	uint32_t uiStartTick;	/*device start indicate time*/
	uint32_t uiLastTick;	/*mark indicate device last Toggle tick*/
	uint32_t uiDuration;	/*device keep indicate time,0 is forever */
	float fFreq;
}IndicateDeviceInfo_t;

typedef struct{
	IndicateDeviceInfo_t xLedR;
	IndicateDeviceInfo_t xLedG;
	IndicateDeviceInfo_t xLedB;
	IndicateDeviceInfo_t xBeep;	
}Indicate_t;





void vIndicateInit(void);
void vIndicate(void);
void vSetIndicate(IndicateDeviceName_t xIndicateDevice,float fFreq,uint32_t uiDuration);
#endif


