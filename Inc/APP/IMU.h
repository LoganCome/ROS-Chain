#ifndef __APP_IMU_H_
#define __APP_IMU_H_
#include <stdint.h>
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h" 

typedef struct{
	float fPitch;
	float fRoll;
	float fYaw;
	float pfEuler[3];
	float pfQuat[4];
	float pfGyro[3];
	float pfAccel[3];
}Imu_t;

extern Imu_t xImu;
extern Imu_t xImuBias;

uint8_t ucImuInit(void);
void vImuValueInit(Imu_t* pxImu);
uint8_t ucGetImuData(Imu_t* pxImu);
uint8_t ucGetImuDataGroup(Imu_t* pxImu);

#endif
