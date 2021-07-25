#ifndef __BSP_BSP_ENCODER_H_
#define __BSP_BSP_ENCODER_H_
#include "main.h"

typedef enum{
	ENCODER1 = 1,
	ENCODER2,	
	ENCODER3,
	ENCODER4
}encoder_t;

int32_t iGetEncoderValue(encoder_t xEncoder);

#endif

