#ifndef ZXCAR_ZET6_CONFIG_H
#define ZXCAR_ZET6_CONFIG_H

#include "stm32f1xx_hal.h"
#include "tim.h"
//#include "i2c.h"
#include "gpio.h"


#ifdef __cplusplus
extern "C" {
#endif
#define PI  3.1415926

//#define KP 1000.0f
//#define KI 5.0f
//#define KD 200.0f

#define KP 1000.0f
#define KI 5.0f
#define KD 200.0f


#define CAR_MODE  2 // 默认是万向轮:1 阿克曼:2
//////////////// Serial Communication ////////////////
#define COMM_HUART huart1
#define COMM_HUART_REF &huart1
#define COMM_HUART_RX huart1.hdmarx
#define COMM_KEY    USART1

//////////////// protocol out ////////////////////////
#define OUT_HEAD_0 0xFE
#define OUT_HEAD_1 0xCE

#define OUT_TYPE_LOG 0xF1
#define OUT_TYPE_IMU 0x11
#define OUT_TYPE_UNIVERSAL_VEL 0x12
#define OUT_TYPE_BATTERY 0x13

//////////////// protocol in ////////////////////////
#define IN_HEAD_0 0xAB
#define IN_HEAD_1 0xBC

#define IN_TYPE_LED 0x01
#define IN_TYPE_BUZZER 0x02
#define IN_TYPE_MOTOR 0x21
#define IN_TYPE_UNIVERSAL_VEL 0x22
#define IN_TYPE_SERVO 0x31

#define IN_LED_CMD_CLOSE 0x00
#define IN_LED_CMD_OPEN 0x01
#define IN_LED_CMD_STATE 0x02

#define IN_BUZZER_CMD_CLOSE 0x00
#define IN_BUZZER_CMD_OPEN 0x01
#define IN_BUZZER_CMD_STATE 0x02

//////////////// Rate //////////////////
#define MOVE_CTRL_RATE 20
#define IMU_PUSH_RATE 20
#define VEL_PUSH_RATE 50

/////////////// LED //////////////////
#define LED_PORT    GPIOE
#define LED_PIN     GPIO_PIN_10

/////////////// Buzzer ///////////////
#define BUZZER_PORT     GPIOB
#define BUZZER_PIN      GPIO_PIN_10

////////////// SWITCH ////////////
#define SWITCH_PORT     GPIOD
#define SWITCH_PIN      GPIO_PIN_8

///////////// Motor back left //////////////////
#define MOTOR1_A_GPIO_PORT     GPIOA
#define MOTOR1_A_PIN           GPIO_PIN_3

#define MOTOR1_B_GPIO_PORT     GPIOA
#define MOTOR1_B_PIN           GPIO_PIN_2

#define MOTOR1_TIM           &htim8
#define MOTOR1_CHANNEL       TIM_CHANNEL_1

#define MOTOR1_DIRECTION    1

///////////// Motor back right //////////////////
#define MOTOR2_A_GPIO_PORT     GPIOA
#define MOTOR2_A_PIN           GPIO_PIN_4

#define MOTOR2_B_GPIO_PORT     GPIOA
#define MOTOR2_B_PIN           GPIO_PIN_5

#define MOTOR2_TIM           &htim8
#define MOTOR2_CHANNEL       TIM_CHANNEL_2

#define MOTOR2_DIRECTION    1


///////////// Motor front right //////////////////
#define MOTOR3_A_GPIO_PORT     GPIOC
#define MOTOR3_A_PIN           GPIO_PIN_5

#define MOTOR3_B_GPIO_PORT     GPIOE
#define MOTOR3_B_PIN           GPIO_PIN_12

#define MOTOR3_TIM           &htim8
#define MOTOR3_CHANNEL       TIM_CHANNEL_3

#define MOTOR3_DIRECTION    -1


///////////// Motor front left //////////////////
#define MOTOR4_A_GPIO_PORT     GPIOB
#define MOTOR4_A_PIN           GPIO_PIN_11

#define MOTOR4_B_GPIO_PORT     GPIOB
#define MOTOR4_B_PIN           GPIO_PIN_15

#define MOTOR4_TIM           &htim8
#define MOTOR4_CHANNEL       TIM_CHANNEL_4

#define MOTOR4_DIRECTION    1

///////////// Encoder back left /////////////////
#define ENCODER1_TIM            &htim2
#define ENCODER1_CHANNEL        TIM_CHANNEL_ALL
#define ENCODER1_DIRECTION      -1

///////////// Encoder back right /////////////////
#define ENCODER2_TIM            &htim3
#define ENCODER2_CHANNEL        TIM_CHANNEL_ALL
#define ENCODER2_DIRECTION      1


///////////// Encoder front right /////////////////
#define ENCODER3_TIM            &htim4
#define ENCODER3_CHANNEL        TIM_CHANNEL_ALL
#define ENCODER3_DIRECTION      -1


///////////// Encoder front left /////////////////
#define ENCODER4_TIM            &htim5
#define ENCODER4_CHANNEL        TIM_CHANNEL_ALL
#define ENCODER4_DIRECTION      1

//////////// Wheel config ///////////////////////
// 轮子转一圈的信号数
#define WHEEL_TPR           (13 * 30 * 4)
// 轮子的直径
#define WHEEL_DIAMETER      0.064f
// 左右轮子的距离
#define WHEEL_DISTANCE      0.155f
// 前后轮子的轴距
#define WHEEL_AXIS          0.1445f

//////////// Servo 1 //////////////////////////// pe13
#define SERVO_K              622.8f
#define SERVO_INIT           1500
#define SERVO1_TIM           &htim1
#define SERVO1_CHANNEL       TIM_CHANNEL_1

/////////// 万向轮PID ///////////////////////////
#define UNIVERSAL_KP    700.0
#define UNIVERSAL_KI    12.0
#define UNIVERSAL_KD    300.0
#define MAX_PWM         7200
#define MIN_PWM         -7200
#define MOVE_DELAY_STOP 1000

////////// 电池 ///////////////////////////////
#define MIN_VOLTAGE         10.7
#define BATTERY_MEASURE_RATE  1
#define BATTERY_CHECK_RATE  0.1

///////// PS2 ////////////////////////////////
#define PS_DI_PORT      GPIOE
#define PS_DI_PIN       GPIO_PIN_15
#define PS_DO_PORT      GPIOD
#define PS_DO_PIN       GPIO_PIN_3
#define PS_CS_PORT      GPIOB
#define PS_CS_PIN       GPIO_PIN_13
#define PS_CLK_PORT     GPIOB
#define PS_CLK_PIN      GPIO_PIN_12
#define PS_TIM          &htim6
#define PS_CHECK_RATE   15

///////// User Key ///////////////////////////
#define USER_KEY_PORT   GPIOD
#define USER_KEY_PIN    GPIO_PIN_8
#define USER_KEY_CHECK_RATE   20
#define MODE_LOOP_RATE  4

#ifdef __cplusplus
}
#endif

#endif //ZXCAR_ZET6_CONFIG_H
