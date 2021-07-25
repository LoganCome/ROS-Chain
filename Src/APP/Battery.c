#include "stm32f1xx_hal.h"
#include "APP/Battery.h"
#include "APP/config.h"

#define ADC_FULL	4096
#define ADC_VOLTAGE	3.3
#define ADC_REF_INIT	(1.2*4096/3.3)

#define VOLTAGE_FILTER_LENGTH	5
#define CURRENT_FILTER_LENGTH	24

extern ADC_HandleTypeDef hadc1;
extern uint8_t pucHardwareVersion[3];
Battery_t xBattery;

uint16_t pusVoltageFilter[VOLTAGE_FILTER_LENGTH] = {0};
uint16_t pusCurrentFilter[CURRENT_FILTER_LENGTH] = {0};
static float fADCRefScale = 1.0;
static uint16_t ucADCRefValue = (uint16_t)ADC_REF_INIT;
float fErrorScale = 1.0;
uint8_t ucUseErrorScale = 1;

static uint16_t  usGetADC1Value(uint32_t ch){
	HAL_StatusTypeDef xHalStatus = HAL_OK;
	ADC_ChannelConfTypeDef ADC1_ChanConf;
	ADC1_ChanConf.Channel = ch;                                  
	ADC1_ChanConf.Rank=1;                                     
	ADC1_ChanConf.SamplingTime = ADC_SAMPLETIME_55CYCLES_5;  
	HAL_ADC_ConfigChannel(&hadc1,&ADC1_ChanConf);      
	HAL_ADC_Start(&hadc1);                              
	xHalStatus = HAL_ADC_PollForConversion(&hadc1,10); 
	if(xHalStatus != HAL_OK)
		printf("usGetADC1Value Faild\r\n");
	return (uint16_t)HAL_ADC_GetValue(&hadc1);
}  

void vADCRefInit(void){
	uint32_t uiTemp = 0;
	uint16_t usTemp = 0;
	/*triple measure to get more accurace value*/
	for(uint8_t i=0;i<32;i++){
		usTemp = usGetADC1Value(ADC_CHANNEL_17);
		uiTemp += usTemp;
	}
	fADCRefScale = (float)ucADCRefValue*32.0/(float)uiTemp;
}

float fGetVoltage(void){
	uint16_t uiADCValue = 0;
	uint16_t usADCValueSum = 0;
	float fVoltage = 0.0;
	uint16_t uiMaxADCValue,uiMinADCValue;
	uiMinADCValue = 4096;
	uiMaxADCValue = 0;
	uiADCValue = usGetADC1Value(VOLTAGE_ADC_CHANNEL);
	for(uint8_t i = 0;i<VOLTAGE_FILTER_LENGTH-1;i++){
		if(pusVoltageFilter[i+1] == 0)
			pusVoltageFilter[i+1] = uiADCValue;
		pusVoltageFilter[i] = pusVoltageFilter[i+1];
		usADCValueSum += pusVoltageFilter[i];
		if(pusVoltageFilter[i] > uiMaxADCValue)	uiMaxADCValue = pusVoltageFilter[i];
		if(pusVoltageFilter[i] < uiMinADCValue)	uiMinADCValue = pusVoltageFilter[i];		
	}
	pusVoltageFilter[VOLTAGE_FILTER_LENGTH-1] = uiADCValue;
	usADCValueSum += pusVoltageFilter[VOLTAGE_FILTER_LENGTH-1];
	
	if(pusVoltageFilter[VOLTAGE_FILTER_LENGTH-1] > uiMaxADCValue)	uiMaxADCValue = pusVoltageFilter[VOLTAGE_FILTER_LENGTH-1];
	if(pusVoltageFilter[VOLTAGE_FILTER_LENGTH-1] < uiMinADCValue)	uiMinADCValue = pusVoltageFilter[VOLTAGE_FILTER_LENGTH-1];
	usADCValueSum -= (uiMaxADCValue + uiMinADCValue);
	uiADCValue = usADCValueSum/(VOLTAGE_FILTER_LENGTH-2);
	
	fVoltage = ADC_VOLTAGE * (float)uiADCValue / (float)ADC_FULL;
	fVoltage *=((VOLTAGE_R1+VOLTAGE_R2)/VOLTAGE_R2);
	fVoltage *= fADCRefScale;
	if(ucUseErrorScale)
		fVoltage *= fErrorScale;
	if(pucHardwareVersion[1] == 0){
		fVoltage += VOLTAGE_ERROR;
	}
	return fVoltage;
}

float fGetCurrent(void){
	uint16_t uiADCValue = 0;
	uint16_t usADCValueSum = 0;
	float fCurrent = 0.0;
	uint16_t uiMaxADCValue,uiMinADCValue;
	uiMinADCValue = 4096;
	uiMaxADCValue = 0;
	uiADCValue = usGetADC1Value(CURRENT_ADC_CHANNEL);
	for(uint8_t i = 0;i<CURRENT_FILTER_LENGTH-1;i++){
		if(pusCurrentFilter[i+1] == 0)
			pusCurrentFilter[i+1] = uiADCValue;
		pusCurrentFilter[i] = pusCurrentFilter[i+1];
		usADCValueSum += pusCurrentFilter[i];
		if(pusCurrentFilter[i] > uiMaxADCValue)	uiMaxADCValue = pusCurrentFilter[i];
		if(pusCurrentFilter[i] < uiMinADCValue)	uiMinADCValue = pusCurrentFilter[i];	
	}
	pusCurrentFilter[CURRENT_FILTER_LENGTH-1] = uiADCValue;
	usADCValueSum += pusCurrentFilter[CURRENT_FILTER_LENGTH-1];
	if(pusCurrentFilter[CURRENT_FILTER_LENGTH-1] > uiMaxADCValue)	uiMaxADCValue = pusCurrentFilter[CURRENT_FILTER_LENGTH-1];
	if(pusCurrentFilter[CURRENT_FILTER_LENGTH-1] < uiMinADCValue)	uiMinADCValue = pusCurrentFilter[CURRENT_FILTER_LENGTH-1];
	
	usADCValueSum -= (uiMaxADCValue + uiMinADCValue);
	uiADCValue = usADCValueSum/(CURRENT_FILTER_LENGTH-2);
	if((pucHardwareVersion[1] == 0) || (pucHardwareVersion[1] == 1) || (pucHardwareVersion[1] == 2)){
		fCurrent = ADC_VOLTAGE * (float)uiADCValue / (float)ADC_FULL;
		fCurrent /= (CURRENT_RES*CURRENT_SCALE);
		fCurrent *= fADCRefScale;
		fCurrent *= fErrorScale;
	}
	if(pucHardwareVersion[1] == 3){
		if(uiADCValue < 1980) uiADCValue = 1980 - uiADCValue;
		else uiADCValue = 0;
		fCurrent = ADC_VOLTAGE * (float)uiADCValue / (float)ADC_FULL;
		fCurrent /= (0.005*50);
		fCurrent *= fADCRefScale;
		fCurrent *= fErrorScale;	
	}
	return fCurrent;
}

float fDoExternalCalibrate(float fVoltage){
	float fVoltageM	= 0.0;
	//disable error scale to get raw voltage
	ucUseErrorScale = 0;
	for(uint8_t i=0;i < 5;i++){
		fVoltageM = fGetVoltage();
	}
	//enable error scale to get calibrated voltage
	ucUseErrorScale = 1;
	return fVoltage/fVoltageM;
}


void vGetBatteryInfo(Battery_t *pxBattery){
	pxBattery->fVoltage = fGetVoltage();
	pxBattery->fCurrent = fGetCurrent();
}


void vGetBatteryVoltageInfo(Battery_t *pxBattery){
	pxBattery->fVoltage = fGetVoltage();
}

void vGetBatteryCurrentInfo(Battery_t *pxBattery){
	pxBattery->fCurrent = fGetCurrent();
}
