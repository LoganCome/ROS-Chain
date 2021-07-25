#ifndef __APP_BATTERY_H_
#define __APP_BATTERY_H_
typedef struct{
	float fVoltage;
	float fCurrent;
}Battery_t;

extern Battery_t xBattery;
extern float fErrorScale;
void vADCRefInit(void);
float fGetVoltage(void);
float fGetCurrent(void);
float fDoExternalCalibrate(float fVoltage);
void vGetBatteryInfo(Battery_t *pxBattery);
void vGetBatteryVoltageInfo(Battery_t *pxBattery);
void vGetBatteryCurrentInfo(Battery_t *pxBattery);
#endif
