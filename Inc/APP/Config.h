#ifndef __APP_CONFIG_H_
#define __APP_CONFIG_H_
#include "main.h"

/*choose base type*/
//#define BASE_TYPE	BASE_2WD
#define BASE_TYPE	BASE_ACKERMAN


#define VERSION_INFO

#ifdef VERSION_INFO
#define SOFTWARE_MAIN_VERSION	1
#define SOFTWARE_SUBVERSION   6
#define SOFTWARE_PATCHLEVEL   0

#define HARDWARE_MAIN_VERSION	1
#define HARDWARE_SUBVERSION   3
#define HARDWARE_PATCHLEVEL   0
#endif

#define BASE_4WD	4
#define BASE_2WD	2
#define BASE_ACKERMAN 1

#define _25GA370	1
#define	_37GB520  2


#define IMU_CHECK_MAX_TRY		2
#define BATTERY_VOLTAGE_CHECK_PERIOD	500
#define BATTERY_CURRENT_CHECK_PERIOD	5
#define MOTOR_CONTROL_PERIOD	20
#define PS2_CHECK_PERIOID			20
#define BATTERY_LOW_VOLTAGE		9.6
#define DISCONNECT_WATI_TIME	1000
#define VELOCITY_LAST_TIME		800
#define SONAR_CHECK_PERIOD	  50

#define MAX_X_VELOCITY	1.0
#define MAX_Y_VELOCITY  0.0
#if BASE_TYPE == BASE_ACKERMAN
#define MAX_Z_ANGLE			0.5
#else
#define MAX_Z_ANGLE			3.14*2
#endif

#define MAX_RPM         10000/(RATIO0+1) //motor's maximum RPM
#define ENCODER     11    // 编码器线数
#if BASE_TYPE == BASE_ACKERMAN
#define RATIO0 30   // 电机减速比
#else
#define RATIO0 21.3   // 电机减速比
#endif
#define COUNTS_PER_REV  ENCODER*RATIO0*4 //wheel encoder's no of ticks per rev(gear_ratio * pulse_per_rev)
#define MS2MINCOV		60000


#define K_P    0.25 // P constant
#define K_I    0.15 // I constant
#define K_D    0.0  // D constant

#define VOLTAGE_ADC_CHANNEL ADC_CHANNEL_8
#define CURRENT_ADC_CHANNEL	ADC_CHANNEL_9

#define VOLTAGE_R1	47.0
#define VOLTAGE_R2	10.0
#define VOLTAGE_ERROR 0.30
#define VOLTAGE_STATE_CHANGE_GAP 0.2

#define CURRENT_RES	0.01
#define CURRENT_SCALE	20

#if BASE_TYPE == BASE_2WD
#define MOTOR_TYPE _25GA370
#else
#define MOTOR_TYPE _37GB520
#endif

#if BASE_TYPE == BASE_ACKERMAN
#define BASE_WIDTH		0.156
#define WHEEL_DIAMETER0  0.065 //wheel's diameter in meters
#else
#define BASE_WIDTH		0.168
#define WHEEL_DIAMETER0  0.067 //wheel's diameter in meters
#endif
#define SONAR_CHANNEL_NUM 0
#endif

