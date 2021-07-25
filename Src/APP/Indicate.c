#include "APP/Indicate.h"

#define FREQ2PERIOD(N)	1000.0/N
 
#define SLOW    0.5 
#define NORMAL  1.0
#define	FAST		2.0
#define RUSH		4.0
 
Indicate_t xIndicate;

/**

*/
static void prvIndicateDeviceInit(IndicateDeviceInfo_t* xDevice,IndicateDeviceName_t xDeviceName){
	xDevice->xDevice = xDeviceName;
	xDevice->fFreq = 0.0;
	xDevice->uiStartTick = 0;
	xDevice->uiLastTick = 0;
	xDevice->uiDuration = 0;
}
/**

*/
static void prvIndicateDeviceSwitch(IndicateDeviceName_t xDeviceName,IndicateDeviceState_t xDeviceState){
	switch (xDeviceName){
		case LEDR:
			HAL_GPIO_WritePin(LED_RED_GPIO_Port,LED_RED_Pin,(GPIO_PinState)xDeviceState);
			break;
		case LEDG:
			HAL_GPIO_WritePin(LED_GREEN_GPIO_Port,LED_GREEN_Pin,(GPIO_PinState)xDeviceState);
			break;
		case LEDB:
			HAL_GPIO_WritePin(LED_BLUE_GPIO_Port,LED_BLUE_Pin,(GPIO_PinState)xDeviceState);
			break;
		case BEEP:
			HAL_GPIO_WritePin(BEEP_GPIO_Port,BEEP_Pin,(GPIO_PinState)xDeviceState);
			break;
		default:
			break;
	}
}
/**

*/
static void prvIndicateDeviceToggle(IndicateDeviceName_t xDeviceName){
	switch (xDeviceName){
		case LEDR:
			HAL_GPIO_TogglePin(LED_RED_GPIO_Port,LED_RED_Pin);
			break;
		case LEDG:
			HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port,LED_GREEN_Pin);
			break;
		case LEDB:
			HAL_GPIO_TogglePin(LED_BLUE_GPIO_Port,LED_BLUE_Pin);
			break;
		case BEEP:
			HAL_GPIO_TogglePin(BEEP_GPIO_Port,BEEP_Pin);
			break;
		default:
			break;
	}
}

/**

*/
void vIndicateInit(void){
	prvIndicateDeviceInit(&xIndicate.xLedR,LEDR);
	prvIndicateDeviceInit(&xIndicate.xLedG,LEDG);
	prvIndicateDeviceInit(&xIndicate.xLedB,LEDB);
	prvIndicateDeviceInit(&xIndicate.xBeep,BEEP);
}
/**

*/
void vIndicate(void){
	uint32_t uiTick = HAL_GetTick();
	IndicateDeviceInfo_t* pxIndicateInfo = NULL;
	pxIndicateInfo = &xIndicate.xLedR;
//	
	for(uint8_t i=0;i<DEVICE_NUM;i++){
		switch(i){
			case LEDR:
				pxIndicateInfo = &xIndicate.xLedR;
				break;
			case LEDG:
				pxIndicateInfo = &xIndicate.xLedG;
				break;
			case LEDB:
				pxIndicateInfo = &xIndicate.xLedB;
				break;
			case BEEP:
				pxIndicateInfo = &xIndicate.xBeep;
				break;
		}
		if(pxIndicateInfo->fFreq > 0){
			if(uiTick - (pxIndicateInfo->uiLastTick)>FREQ2PERIOD(pxIndicateInfo->fFreq)){
				pxIndicateInfo->uiLastTick = uiTick;
				prvIndicateDeviceToggle(pxIndicateInfo->xDevice);
				if((pxIndicateInfo->uiDuration!=0)&&(uiTick - pxIndicateInfo->uiStartTick > pxIndicateInfo->uiDuration)){
					pxIndicateInfo->fFreq = 0;
					prvIndicateDeviceSwitch(pxIndicateInfo->xDevice,INDICATE_DEVICE_OFF);		/*switch device OFF*/
				}
			}
		}
		else{
			prvIndicateDeviceSwitch(pxIndicateInfo->xDevice,INDICATE_DEVICE_OFF);		/*switch device OFF*/
		}
	}
}
/**

*/
void vSetIndicate(IndicateDeviceName_t xIndicateDevice,float fFreq,uint32_t uiDuration){
	IndicateDeviceInfo_t* pxIndicateInfo = NULL;
	switch(xIndicateDevice){
		case LEDR:
			pxIndicateInfo = &xIndicate.xLedR;
			break;
		case LEDG:
			pxIndicateInfo = &xIndicate.xLedG;
			break;
		case LEDB:
			pxIndicateInfo = &xIndicate.xLedB;
			break;
		case BEEP:
			pxIndicateInfo = &xIndicate.xBeep;
			break;
		default:
			break;
	}
	if(pxIndicateInfo!=NULL){
		pxIndicateInfo->fFreq = fFreq;
		pxIndicateInfo->uiDuration = uiDuration;
		pxIndicateInfo->uiStartTick = HAL_GetTick();
	
	}
}
