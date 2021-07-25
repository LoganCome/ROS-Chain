/**
  ******************************************************************************
  * �ļ�����: bsp_eeprom.c 
  * ��    ��: ӲʯǶ��ʽ�����Ŷ�
  * ��    ��: V1.0
  * ��д����: 2015-10-04
  * ��    ��: mpr121�ײ������������OLED_I2C��
  ******************************************************************************
  * ˵����
  * ����������Ӳʯstm32������YS-F1Proʹ�á�
  * 
  * �Ա���
  * ��̳��http://www.ing10bbs.com
  * ��Ȩ��ӲʯǶ��ʽ�����Ŷ����У��������á�
  ******************************************************************************
  */
/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "BSP/oledi2c.h"

/* ˽�����Ͷ��� --------------------------------------------------------------*/
/* ˽�к궨�� ----------------------------------------------------------------*/
/* ˽�б��� ------------------------------------------------------------------*/
/* ��չ���� ------------------------------------------------------------------*/
/* ˽�к���ԭ�� --------------------------------------------------------------*/
/* ������ --------------------------------------------------------------------*/
/**
  * ��������: OLED_I2C����λ�ӳ٣����400KHz
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
static void OLED_I2C_Delay(void)
{
	uint8_t i;

	/*��
	 	�����ʱ����ͨ���߼������ǲ��Եõ��ġ�
		CPU��Ƶ72MHzʱ�����ڲ�Flash����, MDK���̲��Ż�
		ѭ������Ϊ10ʱ��SCLƵ�� = 205KHz 
		ѭ������Ϊ7ʱ��SCLƵ�� = 347KHz�� SCL�ߵ�ƽʱ��1.5us��SCL�͵�ƽʱ��2.87us 
	 	ѭ������Ϊ5ʱ��SCLƵ�� = 421KHz�� SCL�ߵ�ƽʱ��1.25us��SCL�͵�ƽʱ��2.375us 
        
    IAR���̱���Ч�ʸߣ���������Ϊ7
	*/
	for (i = 0; i < 10; i++);
}

/**
  * ��������: CPU����OLED_I2C���������ź�
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
void OLED_I2C_Start(void)
{
	/* ��SCL�ߵ�ƽʱ��SDA����һ�������ر�ʾOLED_I2C���������ź� */
	OLED_I2C_SDA_HIGH();
	OLED_I2C_SCL_HIGH();
	OLED_I2C_Delay();
	OLED_I2C_SDA_LOW();
	OLED_I2C_Delay();
	OLED_I2C_SCL_LOW();
	OLED_I2C_Delay();
}

/**
  * ��������: CPU����OLED_I2C����ֹͣ�ź�
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
void OLED_I2C_Stop(void)
{
	/* ��SCL�ߵ�ƽʱ��SDA����һ�������ر�ʾOLED_I2C����ֹͣ�ź� */
	OLED_I2C_SDA_LOW();
	OLED_I2C_SCL_HIGH();
	OLED_I2C_Delay();
	OLED_I2C_SDA_HIGH();
}

/**
  * ��������: CPU��OLED_I2C�����豸����8bit����
  * �������: Byte �� �ȴ����͵��ֽ�
  * �� �� ֵ: ��
  * ˵    ������
  */
void OLED_I2C_SendByte(uint8_t Byte)
{
	uint8_t i;

	/* �ȷ����ֽڵĸ�λbit7 */
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
			OLED_I2C_SDA_HIGH(); // �ͷ�����
		}
		Byte <<= 1;	/* ����һ��bit */
		OLED_I2C_Delay();
	}
}


/**
  * ��������: CPU��OLED_I2C�����豸��ȡ8bit����
  * �������: ��
  * �� �� ֵ: ����������
  * ˵    ������
  */
uint8_t OLED_I2C_ReadByte(void)
{
	uint8_t i;
	uint8_t value;

	/* ������1��bitΪ���ݵ�bit7 */
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
  * ��������: CPU����һ��ʱ�ӣ�����ȡ������ACKӦ���ź�
  * �������: ��
  * �� �� ֵ: ����0��ʾ��ȷӦ��1��ʾ��������Ӧ
  * ˵    ������
  */
uint8_t OLED_I2C_WaitAck(void)
{
	uint8_t re;

	OLED_I2C_SDA_HIGH();	/* CPU�ͷ�SDA���� */
	OLED_I2C_Delay();
	OLED_I2C_SCL_HIGH();	/* CPU����SCL = 1, ��ʱ�����᷵��ACKӦ�� */
	OLED_I2C_Delay();
	if (OLED_I2C_SDA_READ())	/* CPU��ȡSDA����״̬ */
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
  * ��������: CPU����һ��ACK�ź�
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
void OLED_I2C_Ack(void)
{
	OLED_I2C_SDA_LOW();	/* CPU����SDA = 0 */
	OLED_I2C_Delay();
	OLED_I2C_SCL_HIGH();	/* CPU����1��ʱ�� */
	OLED_I2C_Delay();
	OLED_I2C_SCL_LOW();
	OLED_I2C_Delay();
	OLED_I2C_SDA_HIGH();	/* CPU�ͷ�SDA���� */
}

/**
  * ��������: CPU����1��NACK�ź�
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
void OLED_I2C_NAck(void)
{
	OLED_I2C_SDA_HIGH();	/* CPU����SDA = 1 */
	OLED_I2C_Delay();
	OLED_I2C_SCL_HIGH();	/* CPU����1��ʱ�� */
	OLED_I2C_Delay();
	OLED_I2C_SCL_LOW();
	OLED_I2C_Delay();	
}

/**
  * ��������: ����OLED_I2C���ߵ�GPIO������ģ��IO�ķ�ʽʵ��
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
void OLED_I2C_InitGPIO(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  
  /* ��GPIOʱ�� */
	OLED_I2C_GPIO_CLK_ENABLE();

  GPIO_InitStruct.Pin = OLED_I2C_SCL_PIN|OLED_I2C_SDA_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(OLED_I2C_GPIO_PORT, &GPIO_InitStruct);

  /* ��һ��ֹͣ�ź�, ��λOLED_I2C�����ϵ������豸������ģʽ */
  OLED_I2C_Stop();
}

/**
  * ��������: д�����ݵ�OLED_I2C
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
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
  * ��������: ��OLED_I2C��ȡ����
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
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

/******************* (C) COPYRIGHT 2015-2020 ӲʯǶ��ʽ�����Ŷ� *****END OF FILE****/
