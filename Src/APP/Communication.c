#include <string.h>
#include "APP/Communication.h"
#include "APP/Kinematics.h"
#include "APP/Battery.h"
#include "APP/IMU.h"
#include "APP/config.h"
#include "APP/Motor.h"

#include "main.h"
#define FRAME_HEAD 0x5a
#define DEVICE_ID	 0x01
#define COMMAND_CACHE_SIZE	512
typedef struct{
	uint8_t pucCache[COMMAND_CACHE_SIZE];
	volatile uint16_t ucCacheSize;
	volatile uint16_t ucHead;
	volatile uint16_t ucTail;
	volatile uint16_t ucRemain;
}CommandCache_t;

const uint8_t ucDeviceID = DEVICE_ID;

uint32_t uiGetCacheCounter = 0;
uint32_t uiTakeCacheCounter = 0;

extern Velocity_t xVelocity;
#ifdef VERSION_INFO
extern uint8_t pucSoftwareVersion[3];
extern uint8_t pucHardwareVersion[3];
#endif
extern uint32_t puiUID[3];
extern uint16_t usRatio;
extern float fWheelDiameter;
extern bool bMecanum;
extern Motor_t xMotor1,xMotor2,xMotor3,xMotor4;
extern uint16_t usPosition1,usPosition2,usPosition3,usPosition4;
extern uint8_t ucVelocity1,ucVelocity2,ucVelocity3,ucVelocity4;

extern bool bEmergencyStop;


void vCalculateCRC(void);

static void vSetVelocityCommand(uint8_t* pucCommandBuf)     ;

static void vGetVelocityCommand(void);
static void vGetImuCommand(void);
static void vGetBatteryInfoCommand(void);
static void vGetOdmInfoCommand(void);
static void vGetOdmInfoCommand2(void);
static void vGetRawImuCommand(void);
static void vGetRawImuCommand2(void);
static void vGetVersion(uint8_t *pucHardwareVersion,uint8_t *pucSoftwareVersion);
static void vGetSN(uint32_t *uid);
static void vSetCarVelocityCommand(uint8_t* pucCommandBuf);
static void vGetBaseInfo(uint8_t ucBaseType,uint8_t usMotorType,float fRatio,float fWheelDiameter);
static void vDoReboot(void);
uint8_t ucCommandSendBuf[SEND_COMMAND_LENGTH];
void vGetSonarCommand(void);


static void vSetMotorVelocityCommand(uint8_t* pucCommandBuf);
static void vGetMotorVelocityCommand(void);

/**

*/
static void vFloat2Byte(float *target,unsigned char *buf,unsigned char beg)
{
    unsigned char *point;
    point = (unsigned char*)target;	  //get a float address
    buf[beg]   = point[0];
    buf[beg+1] = point[1];
    buf[beg+2] = point[2];
    buf[beg+3] = point[3];
}


volatile uint8_t ucCommandTempLen = 0;
uint8_t pucCommandTempBuf[32] = {0};
	
volatile uint32_t uiLastVelCmdTick = 0;
static CommandCache_t xCommandCache;

uint8_t pucTestFuckDataBuf[6] = {0};

/**

*/
void vInitCommandCache(void){
	xCommandCache.ucCacheSize = COMMAND_CACHE_SIZE;
	xCommandCache.ucHead = 0;
	xCommandCache.ucTail = 0;
	xCommandCache.ucRemain = COMMAND_CACHE_SIZE;
	memset(xCommandCache.pucCache,0,COMMAND_CACHE_SIZE);
}

/**

*/
__weak void vSendCommand(uint8_t* pucSendBuf,uint8_t ucSendBufLen){

}
/**

*/
void vCalculateCRC(void);
/**

*/
void vDumpCache(void){
	printf("Cache:\r\n");
	for(uint16_t i = 0;i<xCommandCache.ucCacheSize;i++){
		printf("%02x ",xCommandCache.pucCache[i]);
	}
	printf("\r\n");
}
/**
save recv command to circle loop array
*/
uint8_t ucSaveCommandToCache(uint8_t ucCommandLen,uint8_t* pucCommandBuf){
	uint8_t pucTempBuf[UART_RX_BUF_SIZE];
	memcpy(pucTempBuf,pucCommandBuf,UART_RX_BUF_SIZE);
	if(ucCommandLen < xCommandCache.ucRemain){		/*Cache remain size biger than data len*/
		for(uint8_t i=0;i<ucCommandLen;i++){
			xCommandCache.pucCache[xCommandCache.ucTail] = pucTempBuf[i];
			xCommandCache.ucTail = (xCommandCache.ucTail+1)%xCommandCache.ucCacheSize;
			xCommandCache.ucRemain -= 1;
			uiGetCacheCounter++;
		}	
		return 0;
	}
	else{
		printf("Save Cache Faild %d %d %d %d\r\n",xCommandCache.ucHead,xCommandCache.ucTail,xCommandCache.ucRemain,ucCommandLen);
		return 1;
	}
}
/**
take a complete command from command cache
*/
//TODO if data in circle loop array less than command buf need handle
uint8_t usTakeCommandFromCache(void){
	uint8_t ucResult = 1;	
	static uint32_t uiRunTimes = 0;
	if(xCommandCache.ucHead == xCommandCache.ucTail){
		ucResult = 1;
	}
	else{
		uiRunTimes++;
		if(xCommandCache.pucCache[xCommandCache.ucHead] == FRAME_HEAD){
			if(xCommandCache.pucCache[(xCommandCache.ucHead+1)%xCommandCache.ucCacheSize] <= (xCommandCache.ucCacheSize-xCommandCache.ucRemain)){
				ucCommandTempLen = xCommandCache.pucCache[(xCommandCache.ucHead+1)%xCommandCache.ucCacheSize];
				for(uint8_t i=0;i<ucCommandTempLen;i++){
					pucCommandTempBuf[i] = xCommandCache.pucCache[xCommandCache.ucHead];
					xCommandCache.ucHead = (xCommandCache.ucHead+1)%xCommandCache.ucCacheSize;
					xCommandCache.ucRemain += 1;
					uiTakeCacheCounter++;
				}		
				ucResult = 0;
			}
			else{
				ucCommandTempLen = (xCommandCache.ucCacheSize-xCommandCache.ucRemain);
				for(uint8_t i=0;i<ucCommandTempLen;i++){
					pucCommandTempBuf[i] = xCommandCache.pucCache[xCommandCache.ucHead];
					xCommandCache.ucHead = (xCommandCache.ucHead+1)%xCommandCache.ucCacheSize;
					xCommandCache.ucRemain += 1;
					uiTakeCacheCounter++;
				}
				ucResult = 2;
			}
		}
		else{
			xCommandCache.ucHead = (xCommandCache.ucHead+1)%xCommandCache.ucCacheSize;
			xCommandCache.ucRemain += 1;
			ucResult = 3;
		}
	}
	return ucResult;
}
/**

*/
int8_t ucCommandInterpretation(uint8_t ucCommandLen,uint8_t* pucCommandBuf){

	if(pucCommandBuf[0] != FRAME_HEAD){
		HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port,LED_GREEN_Pin);
		return -1;
	}
	if(pucCommandBuf[1] != ucCommandLen){
		HAL_GPIO_TogglePin(LED_BLUE_GPIO_Port,LED_BLUE_Pin);
//		vUSBPrintf("pucCommandBuf[1] %02x \r\n",pucCommandBuf[1]);
		return -1;
	}
	if(pucCommandBuf[ucCommandLen-1] !=crc8_chk_value(pucCommandBuf,ucCommandLen-1) &&(pucCommandBuf[ucCommandLen-1] != 0xff)){
		return -1;
	}
	/*TODO CRC Check is neccessery*/
	if(pucCommandBuf[2] == ucDeviceID){
		switch (pucCommandBuf[3]){
			case 0x01:
				vSetVelocityCommand(pucCommandBuf);			
				break;
			case 0x03:
				vGetVelocityCommand();
				break;
			case 0x05:
				vGetImuCommand();
				break;
			case 0x07:
				vGetBatteryInfoCommand();
				break;
			case 0x09:
				vGetOdmInfoCommand();
				break;
			case 0x11:
				vGetOdmInfoCommand2();
				break;
			case 0x13:
				vGetRawImuCommand();
				break;
			case 0x15:
				vSetCarVelocityCommand(pucCommandBuf);	
				break;
			case 0x19:
				vGetSonarCommand();
				break;
			case 0x21:
				vGetBaseInfo(BASE_TYPE - bMecanum,MOTOR_TYPE,(float)usRatio,fWheelDiameter);
				break;
			case 0x23:
				vGetRawImuCommand2();
				break;
			case 0x31:
				vSetMotorVelocityCommand(pucCommandBuf);
				break;	
			case 0x33:
				vGetMotorVelocityCommand();
				break;				
			case 0xf1:
				#ifdef VERSION_INFO
				vGetVersion(pucHardwareVersion,pucSoftwareVersion);
				#endif
				break;
			case 0xf3:
				vGetSN(puiUID);
				break;
			case 0xfd:
				vDoReboot();
				break;
			default:
				break;
		}
		return pucCommandBuf[ucCommandLen-2];
	}
	else{
		HAL_GPIO_TogglePin(LED_RED_GPIO_Port,LED_RED_Pin);
		return -2;
	}
}
/**

*/
void vSetVelocityCommand(uint8_t* pucCommandBuf){
	uint8_t pucDataBuf[6];
	float fLineX,fLineY,fAngleZ;
	memcpy(pucDataBuf,pucCommandBuf+4,6);
	fLineX = (float)(((int16_t)((pucDataBuf[0]<<8) + pucDataBuf[1]))/1000.0);
	fLineY = (float)(((int16_t)((pucDataBuf[2]<<8) + pucDataBuf[3]))/1000.0);
	fAngleZ = (float)((((int16_t)(pucDataBuf[4]<<8) + pucDataBuf[5]))/1000.0);
	if(bEmergencyStop){
		vComputeMotorSpeed(0.0,0.0,0.0);
	}
	else{
		vComputeMotorSpeed(fLineX,-fLineY,fAngleZ);
	}
	uiLastVelCmdTick = HAL_GetTick();
}

/**

*/
static void vSetCarVelocityCommand(uint8_t* pucCommandBuf){
	uint8_t pucDataBuf[6];
	float fLineX,fAccelX,fAngleZ;
	memcpy(pucDataBuf,pucCommandBuf+4,6);
	fLineX = (float)(((int16_t)((pucDataBuf[0]<<8) + pucDataBuf[1]))/1000.0);
	fAccelX = (float)(((int16_t)((pucDataBuf[2]<<8) + pucDataBuf[3]))/1000.0);
	fAngleZ = (float)((((int16_t)(pucDataBuf[4]<<8) + pucDataBuf[5]))/1000.0);	
	if(bEmergencyStop){
		vComputeCarMotorSpeed(0.0,0.0,0.0);
	}
	else{
		vComputeCarMotorSpeed(fLineX,fAccelX,fAngleZ);	
	}

	uiLastVelCmdTick = HAL_GetTick();
}
/**

*/
void vGetVelocityCommand(void){
	int16_t sLinex,sLiney,sAngleZ;
	sLinex = (int16_t)(xVelocity.fLineX*1000.0);
	sLiney = (int16_t)(xVelocity.fLineY*1000.0);
	sAngleZ = (int16_t)(xVelocity.fAngleZ*1000.0);
	ucCommandSendBuf[0] = FRAME_HEAD;
	ucCommandSendBuf[1] = 12;
	ucCommandSendBuf[2] = ucDeviceID;
	ucCommandSendBuf[3] = 0x04;
	ucCommandSendBuf[4] = (uint8_t)(sLinex>>8);
	ucCommandSendBuf[5] = (uint8_t)(sLinex);
	ucCommandSendBuf[6] = (uint8_t)(sLiney>>8);
	ucCommandSendBuf[7] = (uint8_t)(sLiney);
	ucCommandSendBuf[8] = (uint8_t)(sAngleZ>>8);
	ucCommandSendBuf[9] = (uint8_t)(sAngleZ);
	ucCommandSendBuf[10] = 0x00;
	ucCommandSendBuf[11] = crc8_chk_value(ucCommandSendBuf,11);	
	vSendCommand(ucCommandSendBuf,ucCommandSendBuf[1]);
}
/**

*/
void vGetImuCommand(void){
	int16_t sPitch,sRoll,sYaw;
	sPitch = (int16_t)(xImu.pfEuler[0]);
	sRoll = (int16_t)(xImu.pfEuler[1]);
	sYaw = (int16_t)(xImu.pfEuler[2]);
	ucCommandSendBuf[0] = FRAME_HEAD;
	ucCommandSendBuf[1] = 12;
	ucCommandSendBuf[2] = ucDeviceID;
	ucCommandSendBuf[3] = 0x06;
	ucCommandSendBuf[4] = (uint8_t)(sPitch>>8);
	ucCommandSendBuf[5] = (uint8_t)(sPitch);
	ucCommandSendBuf[6] = (uint8_t)(sRoll>>8);
	ucCommandSendBuf[7] = (uint8_t)(sRoll);
	ucCommandSendBuf[8] = (uint8_t)(sYaw>>8);
	ucCommandSendBuf[9] = (uint8_t)(sYaw);
	ucCommandSendBuf[10] = 0x00;
	ucCommandSendBuf[11] = crc8_chk_value(ucCommandSendBuf,11);	
	vSendCommand(ucCommandSendBuf,ucCommandSendBuf[1]);
}
/**

*/
void vGetBatteryInfoCommand(void){
	uint16_t usVoltage,usCUrrent;
	usVoltage = (uint16_t)(xBattery.fVoltage*1000);
	usCUrrent = (uint16_t)(xBattery.fCurrent*1000);
	ucCommandSendBuf[0] = FRAME_HEAD;
	ucCommandSendBuf[1] = 10;
	ucCommandSendBuf[2] = ucDeviceID;
	ucCommandSendBuf[3] = 0x08;
	ucCommandSendBuf[4] = (uint8_t)(usVoltage>>8);
	ucCommandSendBuf[5] = (uint8_t)(usVoltage);
	ucCommandSendBuf[6] = (uint8_t)(usCUrrent>>8);
	ucCommandSendBuf[7] = (uint8_t)(usCUrrent);
	ucCommandSendBuf[8] = 0;
	ucCommandSendBuf[9] = crc8_chk_value(ucCommandSendBuf,9);	
	vSendCommand(ucCommandSendBuf,ucCommandSendBuf[1]);
}
/**

*/
void vGetOdmInfoCommand(void){
	int16_t sLinex,sAngleZ,sYaw;
	sLinex = (int16_t)(xVelocity.fLineX*1000.0);
	sAngleZ = (int16_t)(xVelocity.fAngleZ*1000.0);
	sYaw = (int16_t)(xImu.pfEuler[2])*100;
	ucCommandSendBuf[0] = FRAME_HEAD;
	ucCommandSendBuf[1] = 12;
	ucCommandSendBuf[2] = ucDeviceID;
	ucCommandSendBuf[3] = 0x0a;
	ucCommandSendBuf[4] = (uint8_t)(sLinex>>8);
	ucCommandSendBuf[5] = (uint8_t)(sLinex);
	ucCommandSendBuf[6] = (uint8_t)(sYaw>>8);
	ucCommandSendBuf[7] = (uint8_t)(sYaw);
	ucCommandSendBuf[8] = (uint8_t)(sAngleZ>>8);
	ucCommandSendBuf[9] = (uint8_t)(sAngleZ);
	ucCommandSendBuf[10] = 0x00;
	ucCommandSendBuf[11] = crc8_chk_value(ucCommandSendBuf,11);
	vSendCommand(ucCommandSendBuf,ucCommandSendBuf[1]);
}
/**

*/
void vGetOdmInfoCommand2(void){
	int16_t sLinex,sLiney,sAngleZ,sYaw;
	sLinex = (int16_t)(xVelocity.fLineX*1000.0);
	sLiney = (int16_t)(xVelocity.fLineY*1000.0);
	sAngleZ = (int16_t)(xVelocity.fAngleZ*1000.0);
	sYaw = (int16_t)(xImu.pfEuler[2])*100;
	ucCommandSendBuf[0] = FRAME_HEAD;
	ucCommandSendBuf[1] = 14;
	ucCommandSendBuf[2] = ucDeviceID;
	ucCommandSendBuf[3] = 0x12;
	ucCommandSendBuf[4] = (uint8_t)(sLinex>>8);
	ucCommandSendBuf[5] = (uint8_t)(sLinex);
	ucCommandSendBuf[6] = (uint8_t)(sLiney>>8);
	ucCommandSendBuf[7] = (uint8_t)(sLiney);
	ucCommandSendBuf[8] = (uint8_t)(sYaw>>8);
	ucCommandSendBuf[9] = (uint8_t)(sYaw);
	ucCommandSendBuf[10] = (uint8_t)(sAngleZ>>8);
	ucCommandSendBuf[11] = (uint8_t)(sAngleZ);
	ucCommandSendBuf[12] = 0x00;
	ucCommandSendBuf[13] = crc8_chk_value(ucCommandSendBuf,13);
	vSendCommand(ucCommandSendBuf,ucCommandSendBuf[1]);
}
/**

*/
void vGetVersion(uint8_t *pucHardwareVersion,uint8_t *pucSoftwareVersion){
	ucCommandSendBuf[0] = FRAME_HEAD;
	ucCommandSendBuf[1] = 12;
	ucCommandSendBuf[2] = ucDeviceID;
	ucCommandSendBuf[3] = 0xf2;
	ucCommandSendBuf[4] = pucHardwareVersion[0];
	ucCommandSendBuf[5] = pucHardwareVersion[1];
	ucCommandSendBuf[6] = pucHardwareVersion[2];
	ucCommandSendBuf[7] = pucSoftwareVersion[0];
	ucCommandSendBuf[8] = pucSoftwareVersion[1];
	ucCommandSendBuf[9] = pucSoftwareVersion[2];
	ucCommandSendBuf[10] = 0x00;
	ucCommandSendBuf[11] = crc8_chk_value(ucCommandSendBuf,11);
	vSendCommand(ucCommandSendBuf,ucCommandSendBuf[1]);
}
/**

*/
void vGetSN(uint32_t *uid){
	ucCommandSendBuf[0] = FRAME_HEAD;
	ucCommandSendBuf[1] = 18;
	ucCommandSendBuf[2] = ucDeviceID;
	ucCommandSendBuf[3] = 0xf4;
	
	ucCommandSendBuf[4] = (uint8_t)(puiUID[0]>>24);
	ucCommandSendBuf[5] = (uint8_t)(puiUID[0]>>16);
	ucCommandSendBuf[6] = (uint8_t)(puiUID[0]>>8);
	ucCommandSendBuf[7] = (uint8_t)(puiUID[0]);

	ucCommandSendBuf[8] = (uint8_t)(puiUID[1]>>24);
	ucCommandSendBuf[9] = (uint8_t)(puiUID[1]>>16);
	ucCommandSendBuf[10] = (uint8_t)(puiUID[1]>>8);
	ucCommandSendBuf[11] = (uint8_t)(puiUID[1]);
	
	ucCommandSendBuf[12] = (uint8_t)(puiUID[2]>>24);
	ucCommandSendBuf[13] = (uint8_t)(puiUID[2]>>16);
	ucCommandSendBuf[14] = (uint8_t)(puiUID[2]>>8);
	ucCommandSendBuf[15] = (uint8_t)(puiUID[2]);
	
	ucCommandSendBuf[16] = 0x00;
	ucCommandSendBuf[17] = crc8_chk_value(ucCommandSendBuf,17);
	vSendCommand(ucCommandSendBuf,ucCommandSendBuf[1]);
}
/**

*/
void vDoReboot(void){
	HAL_NVIC_SystemReset();
}
/**

*/
void vGetRawImuCommand(void){
	int32_t iGyroX = xImu.pfGyro[0]*100000;
	int32_t iGyroY = xImu.pfGyro[1]*100000;
	int32_t iGyroZ = xImu.pfGyro[2]*100000;

	int32_t iAccelX = xImu.pfAccel[0]*100000;
	int32_t iAccelY = xImu.pfAccel[1]*100000;
	int32_t iAccelZ = xImu.pfAccel[2]*100000;
	
	int16_t iQuatW = xImu.pfQuat[0]*10000;
	int16_t iQuatX = xImu.pfQuat[1]*10000;
	int16_t iQuatY = xImu.pfQuat[2]*10000;
	int16_t iQuatZ = xImu.pfQuat[3]*10000;
	
	ucCommandSendBuf[0] = FRAME_HEAD;
	ucCommandSendBuf[1] = 38;
	ucCommandSendBuf[2] = ucDeviceID;
	ucCommandSendBuf[3] = 0x14;
	
	ucCommandSendBuf[4] = (uint8_t)(iGyroX>>24);	
	ucCommandSendBuf[5] = (uint8_t)(iGyroX>>16);	
	ucCommandSendBuf[6] = (uint8_t)(iGyroX>>8);
	ucCommandSendBuf[7] = (uint8_t)(iGyroX);
	
	ucCommandSendBuf[8] = (uint8_t)(iGyroY>>24);	
	ucCommandSendBuf[9] = (uint8_t)(iGyroY>>16);	
	ucCommandSendBuf[10] = (uint8_t)(iGyroY>>8);
	ucCommandSendBuf[11] = (uint8_t)(iGyroY);	
	
	ucCommandSendBuf[12] = (uint8_t)(iGyroZ>>24);	
	ucCommandSendBuf[13] = (uint8_t)(iGyroZ>>16);	
	ucCommandSendBuf[14] = (uint8_t)(iGyroZ>>8);
	ucCommandSendBuf[15] = (uint8_t)(iGyroZ);
	
	
	ucCommandSendBuf[16] = (uint8_t)(iAccelX>>24);	
	ucCommandSendBuf[17] = (uint8_t)(iAccelX>>16);	
	ucCommandSendBuf[18] = (uint8_t)(iAccelX>>8);
	ucCommandSendBuf[19] = (uint8_t)(iAccelX);	
	
	ucCommandSendBuf[20] = (uint8_t)(iAccelY>>24);	
	ucCommandSendBuf[21] = (uint8_t)(iAccelY>>16);	
	ucCommandSendBuf[22] = (uint8_t)(iAccelY>>8);
	ucCommandSendBuf[23] = (uint8_t)(iAccelY);	
	
	ucCommandSendBuf[24] = (uint8_t)(iAccelZ>>24);	
	ucCommandSendBuf[25] = (uint8_t)(iAccelZ>>16);	
	ucCommandSendBuf[26] = (uint8_t)(iAccelZ>>8);
	ucCommandSendBuf[27] = (uint8_t)(iAccelZ);
	
	ucCommandSendBuf[28] = (uint8_t)(iQuatW>>8);
	ucCommandSendBuf[29] = (uint8_t)(iQuatW);

	ucCommandSendBuf[30] = (uint8_t)(iQuatX>>8);
	ucCommandSendBuf[31] = (uint8_t)(iQuatX);
	
	ucCommandSendBuf[32] = (uint8_t)(iQuatY>>8);
	ucCommandSendBuf[33] = (uint8_t)(iQuatY);
	
	ucCommandSendBuf[34] = (uint8_t)(iQuatZ>>8);
	ucCommandSendBuf[35] = (uint8_t)(iQuatZ);
	
	ucCommandSendBuf[36] = 0x00;
	ucCommandSendBuf[37] = crc8_chk_value(ucCommandSendBuf,37);	
	vSendCommand(ucCommandSendBuf,ucCommandSendBuf[1]);
}
/**

*/
void vGetRawImuCommand2(void){
	float fGyroX = xImu.pfGyro[0];
	float fGyroY = xImu.pfGyro[1];
	float fGyroZ = xImu.pfGyro[2];

	float fAccelX = xImu.pfAccel[0];
	float fAccelY = xImu.pfAccel[1];
	float fAccelZ = xImu.pfAccel[2];

	float fQuatW = xImu.pfQuat[0];
	float fQuatX = xImu.pfQuat[1];
	float fQuatY = xImu.pfQuat[2];
	float fQuatZ = xImu.pfQuat[3];
		
	ucCommandSendBuf[0] = FRAME_HEAD;
	ucCommandSendBuf[1] = 46;
	ucCommandSendBuf[2] = ucDeviceID;
	ucCommandSendBuf[3] = 0x24;
	
	vFloat2Byte(&fGyroX,ucCommandSendBuf,4);
	vFloat2Byte(&fGyroY,ucCommandSendBuf,8);
	vFloat2Byte(&fGyroZ,ucCommandSendBuf,12);
	
	vFloat2Byte(&fAccelX,ucCommandSendBuf,16);
	vFloat2Byte(&fAccelY,ucCommandSendBuf,20);
	vFloat2Byte(&fAccelZ,ucCommandSendBuf,24);
	
	vFloat2Byte(&fQuatW,ucCommandSendBuf,28);
	vFloat2Byte(&fQuatX,ucCommandSendBuf,32);
	vFloat2Byte(&fQuatY,ucCommandSendBuf,36);
	vFloat2Byte(&fQuatZ,ucCommandSendBuf,40);
	
	ucCommandSendBuf[44] = 0x00;
	ucCommandSendBuf[45] = crc8_chk_value(ucCommandSendBuf,45);	
		vSendCommand(ucCommandSendBuf,ucCommandSendBuf[1]);		
}
/**

*/
void vSetMotorVelocityCommand(uint8_t* pucCommandBuf){
	uint8_t pucDataBuf[8];
	memcpy(pucDataBuf,pucCommandBuf+4,8);

	xMotor1.sTargetRpm = (int16_t)(((int16_t)((pucDataBuf[0]<<8) + pucDataBuf[1]))/10);
	xMotor2.sTargetRpm = (int16_t)(((int16_t)((pucDataBuf[2]<<8) + pucDataBuf[3]))/10);
	#if BASE_TYPE == BASE_4WD
	xMotor3.sTargetRpm = (int16_t)((((int16_t)(pucDataBuf[4]<<8) + pucDataBuf[5]))/10);	
	xMotor4.sTargetRpm = (int16_t)((((int16_t)(pucDataBuf[6]<<8) + pucDataBuf[7]))/10);
	#endif
	uiLastVelCmdTick = HAL_GetTick();
}
/**

*/
void vGetMotorVelocityCommand(void){
	ucCommandSendBuf[0] = FRAME_HEAD;
	ucCommandSendBuf[1] = 14;
	ucCommandSendBuf[2] = ucDeviceID;
	ucCommandSendBuf[3] = 0x34;
	ucCommandSendBuf[4] = (uint8_t)((xMotor1.sCurrentRpm*10)>>8);
	ucCommandSendBuf[5] = (uint8_t)((xMotor1.sCurrentRpm*10));
	ucCommandSendBuf[6] = (uint8_t)((xMotor2.sCurrentRpm*10)>>8);
	ucCommandSendBuf[7] = (uint8_t)((xMotor2.sCurrentRpm*10));
	#if BASE_TYPE == BASE_4WD
	ucCommandSendBuf[8] = (uint8_t)((xMotor3.sCurrentRpm*10)>>8);
	ucCommandSendBuf[9] = (uint8_t)((xMotor3.sCurrentRpm*10));
	ucCommandSendBuf[10] = (uint8_t)((xMotor4.sCurrentRpm*10)>>8);
	ucCommandSendBuf[11] = (uint8_t)((xMotor4.sCurrentRpm*10));
	#endif
	ucCommandSendBuf[12] = 0x00;
	ucCommandSendBuf[13] = crc8_chk_value(ucCommandSendBuf,13);
	vSendCommand(ucCommandSendBuf,ucCommandSendBuf[1]);
}
/**

*/
void vGetBaseInfo(uint8_t ucBaseType,uint8_t usMotorType,float fRatio,float fDiameter){
	uint16_t usRatio = fRatio * 10;
	uint16_t usWheelDiameter = fDiameter *1000 * 10; //convert m to mm
	ucCommandSendBuf[0] = FRAME_HEAD;
	ucCommandSendBuf[1] = 12;
	ucCommandSendBuf[2] = ucDeviceID;
	ucCommandSendBuf[3] = 0x22;
	ucCommandSendBuf[4] = ucBaseType;
	ucCommandSendBuf[5] = usMotorType;
	ucCommandSendBuf[6] = (uint8_t)(usRatio>>8);
	ucCommandSendBuf[7] = (uint8_t)(usRatio);
	ucCommandSendBuf[8] = (uint8_t)(usWheelDiameter>>8);
	ucCommandSendBuf[9] = (uint8_t)(usWheelDiameter);
	ucCommandSendBuf[10] = 0x00;
	ucCommandSendBuf[11] = crc8_chk_value(ucCommandSendBuf,11);
	vSendCommand(ucCommandSendBuf,ucCommandSendBuf[1]);
}
/**

*/
void vGetSonarCommand(void){
	#if SONAR_CHANNEL_NUM > 0
	ucCommandSendBuf[0] = FRAME_HEAD;
	ucCommandSendBuf[1] = 10;
	ucCommandSendBuf[2] = ucDeviceID;
	ucCommandSendBuf[3] = 0x1a;
	for(uint8_t i = 0; i < 4;i++ ){
		if(uiSonarus[i] < 10000 && uiSonarus[i] > 0){
			ucCommandSendBuf[4+i] = (uint8_t)(uiSonarus[i]*0.017);
		}
		else{
			ucCommandSendBuf[4+i] = 0xff;
		}
	}
	ucCommandSendBuf[8] = 0x00;
	ucCommandSendBuf[9] = crc8_chk_value(ucCommandSendBuf,9);
	vSendCommand(ucCommandSendBuf,ucCommandSendBuf[1]);
	#endif
}
/**

*/
unsigned char crc8_chk_value(unsigned char *message, unsigned char len){
	uint8_t crc;
	uint8_t i;
	crc = 0;
	while(len--){
		crc ^= *message++;
		for(i = 0;i < 8;i++){
			if(crc & 0x01)
			{
				crc = (crc >> 1) ^ 0x8c;
			}
			else crc >>= 1;
		}
	}
	return crc; 
}






