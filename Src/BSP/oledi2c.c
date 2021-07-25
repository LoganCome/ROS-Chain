/**
  ******************************************************************************
  * 文件名程: bsp_eeprom.c 
  * 作    者: 硬石嵌入式开发团队
  * 版    本: V1.0
  * 编写日期: 2015-10-04
  * 功    能: mpr121底层驱动程序（软件OLED_I2C）
  ******************************************************************************
  * 说明：
  * 本例程配套硬石stm32开发板YS-F1Pro使用。
  * 
  * 淘宝：
  * 论坛：http://www.ing10bbs.com
  * 版权归硬石嵌入式开发团队所有，请勿商用。
  ******************************************************************************
  */
/* 包含头文件 ----------------------------------------------------------------*/
#include "BSP/oledi2c.h"

/* 私有类型定义 --------------------------------------------------------------*/
/* 私有宏定义 ----------------------------------------------------------------*/
/* 私有变量 ------------------------------------------------------------------*/
/* 扩展变量 ------------------------------------------------------------------*/
/* 私有函数原形 --------------------------------------------------------------*/
/* 函数体 --------------------------------------------------------------------*/
/**
  * 函数功能: OLED_I2C总线位延迟，最快400KHz
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
static void OLED_I2C_Delay(void)
{
	uint8_t i;

	/*　
	 	下面的时间是通过逻辑分析仪测试得到的。
		CPU主频72MHz时，在内部Flash运行, MDK工程不优化
		循环次数为10时，SCL频率 = 205KHz 
		循环次数为7时，SCL频率 = 347KHz， SCL高电平时间1.5us，SCL低电平时间2.87us 
	 	循环次数为5时，SCL频率 = 421KHz， SCL高电平时间1.25us，SCL低电平时间2.375us 
        
    IAR工程编译效率高，不能设置为7
	*/
	for (i = 0; i < 10; i++);
}

/**
  * 函数功能: CPU发起OLED_I2C总线启动信号
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
void OLED_I2C_Start(void)
{
	/* 当SCL高电平时，SDA出现一个下跳沿表示OLED_I2C总线启动信号 */
	OLED_I2C_SDA_HIGH();
	OLED_I2C_SCL_HIGH();
	OLED_I2C_Delay();
	OLED_I2C_SDA_LOW();
	OLED_I2C_Delay();
	OLED_I2C_SCL_LOW();
	OLED_I2C_Delay();
}

/**
  * 函数功能: CPU发起OLED_I2C总线停止信号
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
void OLED_I2C_Stop(void)
{
	/* 当SCL高电平时，SDA出现一个上跳沿表示OLED_I2C总线停止信号 */
	OLED_I2C_SDA_LOW();
	OLED_I2C_SCL_HIGH();
	OLED_I2C_Delay();
	OLED_I2C_SDA_HIGH();
}

/**
  * 函数功能: CPU向OLED_I2C总线设备发送8bit数据
  * 输入参数: Byte ： 等待发送的字节
  * 返 回 值: 无
  * 说    明：无
  */
void OLED_I2C_SendByte(uint8_t Byte)
{
	uint8_t i;

	/* 先发送字节的高位bit7 */
	for (i = 0; i < 8; i++)
	{		
		if (Byte & 0x80)
		{
			OLED_I2C_SDA_HIGH();
		}
		else
		{
			OLED_I2C_SDA_LOW();
		}
		OLED_I2C_Delay();
		OLED_I2C_SCL_HIGH();
		OLED_I2C_Delay();	
		OLED_I2C_SCL_LOW();
		if (i == 7)
		{
			OLED_I2C_SDA_HIGH(); // 释放总线
		}
		Byte <<= 1;	/* 左移一个bit */
		OLED_I2C_Delay();
	}
}


/**
  * 函数功能: CPU从OLED_I2C总线设备读取8bit数据
  * 输入参数: 无
  * 返 回 值: 读到的数据
  * 说    明：无
  */
uint8_t OLED_I2C_ReadByte(void)
{
	uint8_t i;
	uint8_t value;

	/* 读到第1个bit为数据的bit7 */
	value = 0;
	for (i = 0; i < 8; i++)
	{
		value <<= 1;
		OLED_I2C_SCL_HIGH();
		OLED_I2C_Delay();
		if (OLED_I2C_SDA_READ())
		{
			value++;
		}
		OLED_I2C_SCL_LOW();
		OLED_I2C_Delay();
	}
	return value;
}

/**
  * 函数功能: CPU产生一个时钟，并读取器件的ACK应答信号
  * 输入参数: 无
  * 返 回 值: 返回0表示正确应答，1表示无器件响应
  * 说    明：无
  */
uint8_t OLED_I2C_WaitAck(void)
{
	uint8_t re;

	OLED_I2C_SDA_HIGH();	/* CPU释放SDA总线 */
	OLED_I2C_Delay();
	OLED_I2C_SCL_HIGH();	/* CPU驱动SCL = 1, 此时器件会返回ACK应答 */
	OLED_I2C_Delay();
	if (OLED_I2C_SDA_READ())	/* CPU读取SDA口线状态 */
	{
		re = 1;
	}
	else
	{
		re = 0;
	}
	OLED_I2C_SCL_LOW();
	OLED_I2C_Delay();
	return re;
}

/**
  * 函数功能: CPU产生一个ACK信号
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
void OLED_I2C_Ack(void)
{
	OLED_I2C_SDA_LOW();	/* CPU驱动SDA = 0 */
	OLED_I2C_Delay();
	OLED_I2C_SCL_HIGH();	/* CPU产生1个时钟 */
	OLED_I2C_Delay();
	OLED_I2C_SCL_LOW();
	OLED_I2C_Delay();
	OLED_I2C_SDA_HIGH();	/* CPU释放SDA总线 */
}

/**
  * 函数功能: CPU产生1个NACK信号
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
void OLED_I2C_NAck(void)
{
	OLED_I2C_SDA_HIGH();	/* CPU驱动SDA = 1 */
	OLED_I2C_Delay();
	OLED_I2C_SCL_HIGH();	/* CPU产生1个时钟 */
	OLED_I2C_Delay();
	OLED_I2C_SCL_LOW();
	OLED_I2C_Delay();	
}

/**
  * 函数功能: 配置OLED_I2C总线的GPIO，采用模拟IO的方式实现
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
void OLED_I2C_InitGPIO(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  
  /* 打开GPIO时钟 */
	OLED_I2C_GPIO_CLK_ENABLE();

  GPIO_InitStruct.Pin = OLED_I2C_SCL_PIN|OLED_I2C_SDA_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(OLED_I2C_GPIO_PORT, &GPIO_InitStruct);

  /* 给一个停止信号, 复位OLED_I2C总线上的所有设备到待机模式 */
  OLED_I2C_Stop();
}

/**
  * 函数功能: 写入数据到OLED_I2C
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
void write_OLED_I2C(uint8_t device,uint8_t addr,uint8_t dat)
{
	OLED_I2C_Start();
	OLED_I2C_SendByte(device);
	OLED_I2C_WaitAck();
	OLED_I2C_SendByte(addr);
	OLED_I2C_WaitAck();
	OLED_I2C_SendByte(dat);
	OLED_I2C_WaitAck();
	OLED_I2C_Stop();
}

/**
  * 函数功能: 从OLED_I2C读取数据
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
uint8_t read_OLED_I2C(uint8_t device,uint8_t addr)
{
	uint8_t dat;
	OLED_I2C_Start();
	OLED_I2C_SendByte(device);
	OLED_I2C_WaitAck();
	OLED_I2C_SendByte(addr);
	OLED_I2C_WaitAck();
	OLED_I2C_Start();
	OLED_I2C_SendByte(device+0x01);
	OLED_I2C_WaitAck();
	dat=OLED_I2C_ReadByte();
	OLED_I2C_NAck();
	OLED_I2C_Stop();
	return dat;
}

/******************* (C) COPYRIGHT 2015-2020 硬石嵌入式开发团队 *****END OF FILE****/
