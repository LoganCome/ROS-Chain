//
// Created by Kaijun on 2020/9/11.
//

#include <cstring>
#include "mpu9250.h"
#include "i2c.h"

void Read_DMP(void)
{
/*
 * MPU_Get_Accelerometer(&aacx,&aacy,&aacz);    //�õ����ٶȴ���������
     ACCEL_X=aacx/164;ACCEL_Y=aacy/164;ACCEL_Z=aacz/(-164);
     MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);   //�õ�����������
   GYRO_X=gyrox/(-16.4);GYRO_Y=gyroy/16.4;GYRO_Z=gyroz/(-16.4);
 * */
}
//��ʼ��MPU9250
//����ֵ:0,�ɹ�
//    ����,�������
u8 MPU9250_Init(void)
{
    u8 res=0;
    //IIC_Init();     //��ʼ��IIC����
    MPU_Write_Byte(MPU9250_ADDR,MPU_PWR_MGMT1_REG,0X80);//��λMPU9250

    HAL_Delay(100);  //��ʱ100ms
    MPU_Write_Byte(MPU9250_ADDR,MPU_PWR_MGMT1_REG,0X00);//����MPU9250

    HAL_Delay(100);  //��ʱ100ms
    MPU_Set_Gyro_Fsr(1);                                //�����Ǵ�����,��2000dps
    MPU_Set_Accel_Fsr(0);                               //���ٶȴ�����,��2g
    MPU_Set_Rate(50);                                   //���ò�����50Hz

    MPU_Write_Byte(MPU9250_ADDR,MPU_INT_EN_REG,0X00);   //�ر������ж�
    MPU_Write_Byte(MPU9250_ADDR,MPU_USER_CTRL_REG,0X00);//I2C��ģʽ�ر�
    MPU_Write_Byte(MPU9250_ADDR,MPU_FIFO_EN_REG,0X00);  //�ر�FIFO
    MPU_Write_Byte(MPU9250_ADDR,MPU_INTBP_CFG_REG,0X82);//INT���ŵ͵�ƽ��Ч������bypassģʽ������ֱ�Ӷ�ȡ������
    res=MPU_Read_Byte(MPU9250_ADDR,MPU_DEVICE_ID_REG);  //��ȡMPU6500��ID
    if(res==MPU6500_ID) //����ID��ȷ
    {
        MPU_Write_Byte(MPU9250_ADDR,MPU_PWR_MGMT1_REG,0X01);    //����CLKSEL,PLL X��Ϊ�ο�
        MPU_Write_Byte(MPU9250_ADDR,MPU_PWR_MGMT2_REG,0X00);    //���ٶ��������Ƕ�����
        MPU_Set_Rate(50);                               //���ò�����Ϊ50Hz
    }else return 1;

    res=MPU_Read_Byte(AK8963_ADDR,MAG_WIA);             //��ȡAK8963 ID
    if(res==AK8963_ID)
    {
        MPU_Write_Byte(AK8963_ADDR,MAG_CNTL1,0X11);     //����AK8963Ϊ���β���ģʽ
    }else return 1;

    return 0;
}

//����MPU9250�����Ǵ����������̷�Χ
//fsr:0,��250dps;1,��500dps;2,��1000dps;3,��2000dps
//����ֵ:0,���óɹ�
//    ����,����ʧ��
u8 MPU_Set_Gyro_Fsr(u8 fsr)
{
    return MPU_Write_Byte(MPU9250_ADDR,MPU_GYRO_CFG_REG,fsr<<3);//���������������̷�Χ
}
//����MPU9250���ٶȴ����������̷�Χ
//fsr:0,��2g;1,��4g;2,��8g;3,��16g
//����ֵ:0,���óɹ�
//    ����,����ʧ��
u8 MPU_Set_Accel_Fsr(u8 fsr)
{
    return MPU_Write_Byte(MPU9250_ADDR,MPU_ACCEL_CFG_REG,fsr<<3);//���ü��ٶȴ����������̷�Χ
}

//����MPU9250�����ֵ�ͨ�˲���
//lpf:���ֵ�ͨ�˲�Ƶ��(Hz)
//����ֵ:0,���óɹ�
//    ����,����ʧ��
u8 MPU_Set_LPF(u16 lpf)
{
    u8 data=0;
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
u8 MPU_Set_Rate(u16 rate)
{
    u8 data;
    if(rate>1000)rate=1000;
    if(rate<4)rate=4;
    data=1000/rate-1;
    data=MPU_Write_Byte(MPU9250_ADDR,MPU_SAMPLE_RATE_REG,data); //�������ֵ�ͨ�˲���
    return MPU_Set_LPF(rate/2); //�Զ�����LPFΪ�����ʵ�һ��
}

//�õ��¶�ֵ
//����ֵ:�¶�ֵ(������100��)
short MPU_Get_Temperature(void)
{
    u8 buf[2];
    short raw;
    float temp;
    MPU_Read_Len(MPU9250_ADDR,MPU_TEMP_OUTH_REG,2,buf);
    raw=((u16)buf[0]<<8)|buf[1];
    temp=21+((double)raw)/333.87;
    return temp*100;
}
//�õ�������ֵ(ԭʼֵ)
//gx,gy,gz:������x,y,z���ԭʼ����(������)
//����ֵ:0,�ɹ�
//    ����,�������
short Deviation_gyro[3],Original_gyro[3];
int Deviation_Count;
short gyro[3];
u8 MPU_Get_Gyroscope(short *gx,short *gy,short *gz)
{
    u8 buf[6],res;
    res=MPU_Read_Len(MPU9250_ADDR,MPU_GYRO_XOUTH_REG,6,buf);
    if(res==0)
    {
        if(Deviation_Count<CONTROL_DELAY)//����ǰ����ȡ���������
        {
            Deviation_Count++;
            memcpy(Deviation_gyro,gyro,sizeof(gyro));
            // ��10���ԭʼ����
            gyro[0] = ((u16)buf[0]<<8)|buf[1];
            gyro[1] = ((u16)buf[2]<<8)|buf[3];
            gyro[2] = ((u16)buf[4]<<8)|buf[5];
        }else{
            // ����������ԭʼ����
            Original_gyro[0] = ((u16)buf[0]<<8)|buf[1];
            Original_gyro[1] = ((u16)buf[2]<<8)|buf[3];
            Original_gyro[2] = ((u16)buf[4]<<8)|buf[5];
            // ԭʼ���ݼ�ȥ��ֹʱ��ƫ����
            gyro[0] =Original_gyro[0]-Deviation_gyro[0];  //���մ�ȥ�����Ư�Ƶ�����
            gyro[1] =Original_gyro[1]-Deviation_gyro[1];
            gyro[2]= Original_gyro[2]-Deviation_gyro[2];
        }
        // �����յ����ݷ��س�ȥ
        *gx=gyro[0];
        *gy=gyro[1];
        *gz=gyro[2];
    }
    return res;;
}
//�õ����ٶ�ֵ(ԭʼֵ)
//gx,gy,gz:������x,y,z���ԭʼ����(������)
//����ֵ:0,�ɹ�
//    ����,�������
u8 MPU_Get_Accelerometer(short *ax,short *ay,short *az)
{
    u8 buf[6],res;
    res=MPU_Read_Len(MPU9250_ADDR,MPU_ACCEL_XOUTH_REG,6,buf);
    if(res==0)
    {
        *ax=((u16)buf[0]<<8)|buf[1];
        *ay=((u16)buf[2]<<8)|buf[3];
        *az=((u16)buf[4]<<8)|buf[5];
    }
    return res;;
}

//�õ�������ֵ(ԭʼֵ)
//mx,my,mz:������x,y,z���ԭʼ����(������)
//����ֵ:0,�ɹ�
//    ����,�������
u8 MPU_Get_Magnetometer(short *mx,short *my,short *mz)
{
    u8 buf[6],res;
    res=MPU_Read_Len(AK8963_ADDR,MAG_XOUT_L,6,buf);
    if(res==0)
    {
        *mx=((u16)buf[1]<<8)|buf[0];
        *my=((u16)buf[3]<<8)|buf[2];
        *mz=((u16)buf[5]<<8)|buf[4];
    }
    MPU_Write_Byte(AK8963_ADDR,MAG_CNTL1,0X11); //AK8963ÿ�ζ����Ժ���Ҫ��������Ϊ���β���ģʽ
    return res;;
}

//IIC����д
//addr:������ַ
//reg:�Ĵ�����ַ
//len:д�볤��
//buf:������
//����ֵ:0,����
//    ����,�������
u8 MPU_Write_Len(u8 addr,u8 reg,u8 len,u8 *buf)
{
    HAL_I2C_Mem_Write(&hi2c1,addr<<1,reg,I2C_MEMADD_SIZE_8BIT,buf,len,300);
    /* u8 i;
     IIC_Start();
     IIC_Send_Byte((addr<<1)|0); //����������ַ+д����
     if(IIC_Wait_Ack())          //�ȴ�Ӧ��
     {
         IIC_Stop();
         return 1;
     }
     IIC_Send_Byte(reg);         //д�Ĵ�����ַ
     IIC_Wait_Ack();             //�ȴ�Ӧ��
     for(i=0;i<len;i++)
     {
         IIC_Send_Byte(buf[i]);  //��������
         if(IIC_Wait_Ack())      //�ȴ�ACK
         {
             IIC_Stop();
             return 1;
         }
     }
     IIC_Stop();*/
    return 0;
}

//IIC������
//addr:������ַ
//reg:Ҫ��ȡ�ļĴ�����ַ
//len:Ҫ��ȡ�ĳ���
//buf:��ȡ�������ݴ洢��
//����ֵ:0,����
//    ����,�������
u8 MPU_Read_Len(u8 addr,u8 reg,u8 len,u8 *buf)
{
    HAL_I2C_Mem_Read(&hi2c1, addr<<1, reg, I2C_MEMADD_SIZE_8BIT, buf, len, 300);
    /*IIC_Start();
    IIC_Send_Byte((addr<<1)|0); //����������ַ+д����
    if(IIC_Wait_Ack())          //�ȴ�Ӧ��
    {
        IIC_Stop();
        return 1;
    }
    IIC_Send_Byte(reg);         //д�Ĵ�����ַ
    IIC_Wait_Ack();             //�ȴ�Ӧ��
    IIC_Start();
    IIC_Send_Byte((addr<<1)|1); //����������ַ+������
    IIC_Wait_Ack();             //�ȴ�Ӧ��
    while(len)
    {
        if(len==1)*buf=IIC_Read_Byte(0);//������,����nACK
        else *buf=IIC_Read_Byte(1);     //������,����ACK
        len--;
        buf++;
    }
    IIC_Stop();                 //����һ��ֹͣ����*/
    return 0;
}

//IICдһ���ֽ�
//devaddr:����IIC��ַ
//reg:�Ĵ�����ַ
//data:����
//����ֵ:0,����
//    ����,�������
u8 MPU_Write_Byte(u8 addr,u8 reg,u8 data)
{
    HAL_I2C_Mem_Write(&hi2c1,addr<<1,reg,I2C_MEMADD_SIZE_8BIT,&data,1,300);
//    IIC_Start();
////    IIC_Send_Byte((addr<<1)|0); //����������ַ+д����
////    if(IIC_Wait_Ack())          //�ȴ�Ӧ��
////    {
////        IIC_Stop();
////        return 1;
////    }
////    IIC_Send_Byte(reg);         //д�Ĵ�����ַ
////    IIC_Wait_Ack();             //�ȴ�Ӧ��
////    IIC_Send_Byte(data);        //��������
////    if(IIC_Wait_Ack())          //�ȴ�ACK
////    {
////        IIC_Stop();
////        return 1;
////    }
////    IIC_Stop();
    return 0;
}

//IIC��һ���ֽ�
//reg:�Ĵ�����ַ
//����ֵ:����������
u8 MPU_Read_Byte(u8 addr,u8 reg)
{
    uint8_t res;
    HAL_I2C_Mem_Read(&hi2c1, addr<<1, reg, I2C_MEMADD_SIZE_8BIT, &res, 1, 300);
//    u8 res;
//    IIC_Start();
//    IIC_Send_Byte((addr<<1)|0); //����������ַ+д����
//    IIC_Wait_Ack();             //�ȴ�Ӧ��
//    IIC_Send_Byte(reg);         //д�Ĵ�����ַ
//    IIC_Wait_Ack();             //�ȴ�Ӧ��
//    IIC_Start();
//    IIC_Send_Byte((addr<<1)|1); //����������ַ+������
//    IIC_Wait_Ack();             //�ȴ�Ӧ��
//    res=IIC_Read_Byte(0);     //������,����nACK
//    IIC_Stop();                 //����һ��ֹͣ����

    return res;
}