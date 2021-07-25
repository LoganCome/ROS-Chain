#include "stm32f1xx_hal.h"
#include "app/config.h"
#include "main.h"

extern uint8_t bMpuIntCounter;
/**
  * @brief  EXTI line detection callbacks.
  * @param  GPIO_Pin: Specifies the pins connected EXTI line
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	switch(GPIO_Pin){
		case (MPU_INT_Pin):
			if(bMpuIntCounter < 0xff)	bMpuIntCounter++;
			break;
		#if SONAR_CHANNEL_NUM > 0
		case (SR04_ECHO_1_Pin):
			vEchoSonar(1,HAL_GPIO_ReadPin(SR04_ECHO_1_GPIO_Port,SR04_ECHO_1_Pin ));
			break;
		#endif	
		#if SONAR_CHANNEL_NUM > 1
		case(SR04_ECHO_2_Pin):
			vEchoSonar(2,HAL_GPIO_ReadPin(SR04_ECHO_2_GPIO_Port,SR04_ECHO_2_Pin ));
			break;
		#endif		
		default:
			break;
	}
}
