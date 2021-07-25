#ifndef __MAINLOOP_H_
#define __MAINLOOP_H_

#include "main.h"
#include <string.h>
#include <stdbool.h>

#include "BSP/bsp_encoder.h"
#include "BSP/bsp_motor.h"
#include "BSP/mpu9250.h"

#include "APP/Motor.h"
#include "APP/Battery.h"
#include "APP/Kinematics.h"
#include "APP/Communication.h"
#include "APP/IMU.h"
#include "APP/Config.h"
#include "APP/Indicate.h"
#include "APP/LogModule.h"
#include "APP/PS2.h"

#if BASE_TYPE == BASE_ACKERMAN
//#include "BSP/bsp_servo.h"
#include "APP/servo.h"
#endif

#ifdef VERSION_INFO
extern uint8_t pucSoftwareVersion[3];
extern uint8_t pucHardwareVersion[3];
#endif

extern Motor_t xMotor1,xMotor2,xMotor3,xMotor4;

void vMotorHWInit(void);
void vMainLoop(void);
#endif

