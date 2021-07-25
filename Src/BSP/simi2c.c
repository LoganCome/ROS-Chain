#include "BSP/simi2c.h"

/**

*/
static void I2C_Delay(void){
	uint8_t i;
	/*delay loop, IIC about 347KHz,this is measured by oscilloscope*/
	for (i = 0; i < 7; i++); 
}

/**

*/
void I2C_Start(void){
	I2C_SDA_HIGH();
	I2C_SCL_HIGH();
	I2C_Delay();
	I2C_SDA_LOW();
	I2C_Delay();
	I2C_SCL_LOW();
	I2C_Delay();
}

/**

*/
void I2C_Stop(void){
	I2C_SDA_LOW();
	I2C_SCL_HIGH();
	I2C_Delay();
	I2C_SDA_HIGH();
}

/**

*/
void I2C_SendByte(uint8_t Byte){
	uint8_t i;
	for (i = 0; i < 8; i++)
	{		
		if (Byte & 0x80)
		{
			I2C_SDA_HIGH();
		}
		else
		{
			I2C_SDA_LOW();
		}
		I2C_Delay();
		I2C_SCL_HIGH();
		I2C_Delay();	
		I2C_SCL_LOW();
		if (i == 7)
		{
			I2C_SDA_HIGH(); // releas bus
		}
		Byte <<= 1;	/* move letf one bit  */
		I2C_Delay();
	}
}


/**

*/
uint8_t I2C_ReadByte(uint8_t ack){
	uint8_t i;
	uint8_t value;
	value = 0;
	for (i = 0; i < 8; i++)
	{
		value <<= 1;
		I2C_SCL_HIGH();
		I2C_Delay();
		if (I2C_SDA_READ())
		{
			value++;
		}
		I2C_SCL_LOW();
		I2C_Delay();
	}
  if(ack==0)
		I2C_NAck();
	else
		I2C_Ack();
	return value;
}
/**

*/
uint8_t I2C_WaitAck(void){
	uint8_t re;

	I2C_SDA_HIGH();	
	I2C_Delay();
	I2C_SCL_HIGH();	
	I2C_Delay();
	if (I2C_SDA_READ())
	{
		re = 1;
	}
	else
	{
		re = 0;
	}
	I2C_SCL_LOW();
	I2C_Delay();
	return re;
}
/**

*/
void I2C_Ack(void){
	I2C_SDA_LOW();	
	I2C_Delay();
	I2C_SCL_HIGH();	
	I2C_Delay();
	I2C_SCL_LOW();
	I2C_Delay();
	I2C_SDA_HIGH();
}
/**

*/
void I2C_NAck(void){
	I2C_SDA_HIGH();	
	I2C_Delay();
	I2C_SCL_HIGH();	
	I2C_Delay();
	I2C_SCL_LOW();
	I2C_Delay();	
}

/**

*/
void I2C_InitGPIO(void){
  GPIO_InitTypeDef GPIO_InitStruct;
 
	I2C_GPIO_CLK_ENABLE();

  GPIO_InitStruct.Pin = I2C_SCL_PIN|I2C_SDA_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(I2C_GPIO_PORT, &GPIO_InitStruct);
  I2C_Stop();
}

/**

*/
uint8_t I2C_CheckDevice(uint8_t _Address){
	uint8_t ucAck;

	I2C_InitGPIO();		
	I2C_Start();		
	I2C_SendByte(_Address | I2C_WR);
	ucAck = I2C_WaitAck();
	I2C_Stop();	
	return ucAck;
}
