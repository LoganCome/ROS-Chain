#include "APP/IMU.h"
#include "BSP/mpu9250.h"

Imu_t xImu;
Imu_t xImuBias;

uint8_t ucImuInit(void){
	return mpu_dmp_init();
}

uint8_t ucGetImuData(Imu_t* pxImu){
	uint8_t ucResult = 0;
	float pfTempBuf[3] = {0.0};
	ucResult = mpu_mpl_get_data(SENSOR_TYPE_EULER,pfTempBuf);
	if(0 == ucResult){
		pxImu->fPitch = pfTempBuf[0];
		pxImu->fRoll = pfTempBuf[1];
		pxImu->fYaw = pfTempBuf[2];
	}
	return ucResult;
}

uint8_t ucGetImuDataGroup(Imu_t* pxImu){
	uint8_t ucResult = 0;
	ucResult = mpu_mpl_get_data_group(pxImu->pfEuler,pxImu->pfQuat,pxImu->pfGyro,pxImu->pfAccel);
	xImu.fPitch = xImu.pfEuler[0];
	xImu.fRoll = xImu.pfEuler[1];
	xImu.fYaw = xImu.pfEuler[2];
	return ucResult;	
}


