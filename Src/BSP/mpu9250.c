#include "BSP/mpu9250.h"
#include "BSP/simi2c.h"

//��ʼ��MPU9250
//����ֵ:0,�ɹ�
//    ����,�������
uint8_t MPU9250_Init(void)
{
	uint8_t res=0;
	I2C_InitGPIO();     //��ʼ��I2C����
	MPU_Write_Byte(MPU9250_ADDR,MPU_PWR_MGMT1_REG,0X80);//��λMPU9250
	/*TODO delay time need adjust when support RTOS*/
	HAL_Delay(50);
	MPU_Write_Byte(MPU9250_ADDR,MPU_PWR_MGMT1_REG,0X00);//����MPU9250
	MPU_Set_Gyro_Fsr(3);					        	//�����Ǵ�����,��2000dps
	MPU_Set_Accel_Fsr(0);					       	 	//���ٶȴ�����,��2g
	MPU_Set_Rate(50);						       	 	//���ò�����50Hz
	MPU_Write_Byte(MPU9250_ADDR,MPU_INT_EN_REG,0X00);   //�ر������ж�
	MPU_Write_Byte(MPU9250_ADDR,MPU_USER_CTRL_REG,0X00);//I2C��ģʽ�ر�
	MPU_Write_Byte(MPU9250_ADDR,MPU_FIFO_EN_REG,0X00);	//�ر�FIFO
	MPU_Write_Byte(MPU9250_ADDR,MPU_INTBP_CFG_REG,0X82);//INT���ŵ͵�ƽ��Ч������bypassģʽ������ֱ�Ӷ�ȡ������
	res=MPU_Read_Byte(MPU9250_ADDR,MPU_DEVICE_ID_REG);  //��ȡMPU6500��ID
	if(res==MPU6500_ID) //����ID��ȷ
	{
			MPU_Write_Byte(MPU9250_ADDR,MPU_PWR_MGMT1_REG,0X01);  	//����CLKSEL,PLL X��Ϊ�ο�
			MPU_Write_Byte(MPU9250_ADDR,MPU_PWR_MGMT2_REG,0X00);  	//���ٶ��������Ƕ�����
			MPU_Set_Rate(50);						       	//���ò�����Ϊ50Hz   
	}else return 1;

	res=MPU_Read_Byte(AK8963_ADDR,MAG_WIA);    			//��ȡAK8963 ID   
	if(res==AK8963_ID)
	{
			MPU_Write_Byte(AK8963_ADDR,MAG_CNTL1,0X11);		//����AK8963Ϊ���β���ģʽ
	}else return 1;

	return 0;
}

//����MPU9250�����Ǵ����������̷�Χ
//fsr:0,��250dps;1,��500dps;2,��1000dps;3,��2000dps
//����ֵ:0,���óɹ�
//    ����,����ʧ�� 
uint8_t MPU_Set_Gyro_Fsr(uint8_t fsr)
{
	return MPU_Write_Byte(MPU9250_ADDR,MPU_GYRO_CFG_REG,fsr<<3);//���������������̷�Χ  
}
//����MPU9250���ٶȴ����������̷�Χ
//fsr:0,��2g;1,��4g;2,��8g;3,��16g
//����ֵ:0,���óɹ�
//    ����,����ʧ�� 
uint8_t MPU_Set_Accel_Fsr(uint8_t fsr)
{
	return MPU_Write_Byte(MPU9250_ADDR,MPU_ACCEL_CFG_REG,fsr<<3);//���ü��ٶȴ����������̷�Χ  
}

//����MPU9250�����ֵ�ͨ�˲���
//lpf:���ֵ�ͨ�˲�Ƶ��(Hz)
//����ֵ:0,���óɹ�
//    ����,����ʧ�� 
uint8_t MPU_Set_LPF(uint16_t lpf)
{
	uint8_t data=0;
	if(lpf>=188)data=1;
	else if(lpf>=98)data=2;
	else if(lpf>=42)data=3;
	else if(lpf>=20)data=4;
	else if(lpf>=10)data=5;
	else data=6; 
	return MPU_Write_Byte(MPU9250_ADDR,MPU_CFG_REG,data);//�������ֵ�ͨ�˲���  
}

//����MPU9250�Ĳ�����(�ٶ�Fs=1KHz)
//rate:4~1000(Hz)
//����ֵ:0,���óɹ�
//    ����,����ʧ�� 
uint8_t MPU_Set_Rate(uint16_t rate)
{
	uint8_t data;
	if(rate>1000)rate=1000;
	if(rate<4)rate=4;
	data=1000/rate-1;
	data=MPU_Write_Byte(MPU9250_ADDR,MPU_SAMPLE_RATE_REG,data);	//�������ֵ�ͨ�˲���
 	return MPU_Set_LPF(rate/2);	//�Զ�����LPFΪ�����ʵ�һ��
}

//�õ��¶�ֵ
//����ֵ:�¶�ֵ(������100��)
short MPU_Get_Temperature(void)
{
    uint8_t buf[2]; 
    short raw;
	float temp;
	MPU_Read_Len(MPU9250_ADDR,MPU_TEMP_OUTH_REG,2,buf); 
    raw=((uint16_t)buf[0]<<8)|buf[1];  
    temp=21+((double)raw)/333.87;  
    return temp*100;;
}
//�õ�������ֵ(ԭʼֵ)
//gx,gy,gz:������x,y,z���ԭʼ����(������)
//����ֵ:0,�ɹ�
//    ����,�������
uint8_t MPU_Get_Gyroscope(short *gx,short *gy,short *gz)
{
    uint8_t buf[6],res; 
	res=MPU_Read_Len(MPU9250_ADDR,MPU_GYRO_XOUTH_REG,6,buf);
	if(res==0)
	{
		*gx=((uint16_t)buf[0]<<8)|buf[1];  
		*gy=((uint16_t)buf[2]<<8)|buf[3];  
		*gz=((uint16_t)buf[4]<<8)|buf[5];
	} 	
    return res;;
}
//�õ����ٶ�ֵ(ԭʼֵ)
//gx,gy,gz:������x,y,z���ԭʼ����(������)
//����ֵ:0,�ɹ�
//    ����,�������
uint8_t MPU_Get_Accelerometer(short *ax,short *ay,short *az)
{
    uint8_t buf[6],res;  
	res=MPU_Read_Len(MPU9250_ADDR,MPU_ACCEL_XOUTH_REG,6,buf);
	if(res==0)
	{
		*ax=((uint16_t)buf[0]<<8)|buf[1];  
		*ay=((uint16_t)buf[2]<<8)|buf[3];  
		*az=((uint16_t)buf[4]<<8)|buf[5];
	} 	
    return res;;
}

//�õ�������ֵ(ԭʼֵ)
//mx,my,mz:������x,y,z���ԭʼ����(������)
//����ֵ:0,�ɹ�
//    ����,�������
uint8_t MPU_Get_Magnetometer(short *mx,short *my,short *mz)
{
    uint8_t buf[6],res;  
	res=MPU_Read_Len(AK8963_ADDR,MAG_XOUT_L,6,buf);
	if(res==0)
	{
		*mx=((uint16_t)buf[1]<<8)|buf[0];  
		*my=((uint16_t)buf[3]<<8)|buf[2];  
		*mz=((uint16_t)buf[5]<<8)|buf[4];
	} 	
    MPU_Write_Byte(AK8963_ADDR,MAG_CNTL1,0X11); //AK8963ÿ�ζ����Ժ���Ҫ��������Ϊ���β���ģʽ
    return res;;
}

//I2C����д
//addr:������ַ 
//reg:�Ĵ�����ַ
//len:д�볤��
//buf:������
//����ֵ:0,����
//    ����,�������
uint8_t MPU_Write_Len(uint8_t addr,uint8_t reg,uint8_t len,uint8_t *buf)
{
    uint8_t i;
    I2C_Start();
    I2C_SendByte((addr<<1)|0); //����������ַ+д����
    if(I2C_WaitAck())          //�ȴ�Ӧ��
    {
        I2C_Stop();
        return 1;
    }
    I2C_SendByte(reg);         //д�Ĵ�����ַ
    I2C_WaitAck();             //�ȴ�Ӧ��
    for(i=0;i<len;i++)
    {
        I2C_SendByte(buf[i]);  //��������
        if(I2C_WaitAck())      //�ȴ�ACK
        {
            I2C_Stop();
            return 1;
        }
    }
    I2C_Stop();
    return 0;
} 

//I2C������
//addr:������ַ
//reg:Ҫ��ȡ�ļĴ�����ַ
//len:Ҫ��ȡ�ĳ���
//buf:��ȡ�������ݴ洢��
//����ֵ:0,����
//    ����,�������
uint8_t MPU_Read_Len(uint8_t addr,uint8_t reg,uint8_t len,uint8_t *buf)
{ 
    I2C_Start();
    I2C_SendByte((addr<<1)|0); //����������ַ+д����
    if(I2C_WaitAck())          //�ȴ�Ӧ��
    {
        I2C_Stop();
        return 1;
    }
    I2C_SendByte(reg);         //д�Ĵ�����ַ
    I2C_WaitAck();             //�ȴ�Ӧ��
	I2C_Start();                
    I2C_SendByte((addr<<1)|1); //����������ַ+������
    I2C_WaitAck();             //�ȴ�Ӧ��
    while(len)
    {
        if(len==1)*buf=I2C_ReadByte(0);//������,����nACK 
		else *buf=I2C_ReadByte(1);		//������,����ACK  
		len--;
		buf++;  
    }
    I2C_Stop();                 //����һ��ֹͣ����
    return 0;       
}

//I2Cдһ���ֽ� 
//devaddr:����I2C��ַ
//reg:�Ĵ�����ַ
//data:����
//����ֵ:0,����
//    ����,�������
uint8_t MPU_Write_Byte(uint8_t addr,uint8_t reg,uint8_t data)
{
    I2C_Start();
    I2C_SendByte((addr<<1)|0); //����������ַ+д����
    if(I2C_WaitAck())          //�ȴ�Ӧ��
    {
        I2C_Stop();
        return 1;
    }
    I2C_SendByte(reg);         //д�Ĵ�����ַ
    I2C_WaitAck();             //�ȴ�Ӧ��
    I2C_SendByte(data);        //��������
    if(I2C_WaitAck())          //�ȴ�ACK
    {
        I2C_Stop();
        return 1;
    }
    I2C_Stop();
    return 0;
}

//I2C��һ���ֽ� 
//reg:�Ĵ�����ַ 
//����ֵ:����������
uint8_t MPU_Read_Byte(uint8_t addr,uint8_t reg)
{
    uint8_t res;
    I2C_Start();
    I2C_SendByte((addr<<1)|0); //����������ַ+д����
    I2C_WaitAck();             //�ȴ�Ӧ��
    I2C_SendByte(reg);         //д�Ĵ�����ַ
    I2C_WaitAck();             //�ȴ�Ӧ��
	  I2C_Start();                
    I2C_SendByte((addr<<1)|1); //����������ַ+������
    I2C_WaitAck();             //�ȴ�Ӧ��
    res=I2C_ReadByte(0);		//������,����nACK  
    I2C_Stop();                 //����һ��ֹͣ����
    return res;  
}
