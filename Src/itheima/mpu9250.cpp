//
// Created by Kaijun on 2020/9/11.
//

#include <cstring>
#include "mpu9250.h"
#include "i2c.h"

void Read_DMP(void)
{
/*
 * MPU_Get_Accelerometer(&aacx,&aacy,&aacz);    //得到加速度传感器数据
     ACCEL_X=aacx/164;ACCEL_Y=aacy/164;ACCEL_Z=aacz/(-164);
     MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);   //得到陀螺仪数据
   GYRO_X=gyrox/(-16.4);GYRO_Y=gyroy/16.4;GYRO_Z=gyroz/(-16.4);
 * */
}
//初始化MPU9250
//返回值:0,成功
//    其他,错误代码
u8 MPU9250_Init(void)
{
    u8 res=0;
    //IIC_Init();     //初始化IIC总线
    MPU_Write_Byte(MPU9250_ADDR,MPU_PWR_MGMT1_REG,0X80);//复位MPU9250

    HAL_Delay(100);  //延时100ms
    MPU_Write_Byte(MPU9250_ADDR,MPU_PWR_MGMT1_REG,0X00);//唤醒MPU9250

    HAL_Delay(100);  //延时100ms
    MPU_Set_Gyro_Fsr(1);                                //陀螺仪传感器,±2000dps
    MPU_Set_Accel_Fsr(0);                               //加速度传感器,±2g
    MPU_Set_Rate(50);                                   //设置采样率50Hz

    MPU_Write_Byte(MPU9250_ADDR,MPU_INT_EN_REG,0X00);   //关闭所有中断
    MPU_Write_Byte(MPU9250_ADDR,MPU_USER_CTRL_REG,0X00);//I2C主模式关闭
    MPU_Write_Byte(MPU9250_ADDR,MPU_FIFO_EN_REG,0X00);  //关闭FIFO
    MPU_Write_Byte(MPU9250_ADDR,MPU_INTBP_CFG_REG,0X82);//INT引脚低电平有效，开启bypass模式，可以直接读取磁力计
    res=MPU_Read_Byte(MPU9250_ADDR,MPU_DEVICE_ID_REG);  //读取MPU6500的ID
    if(res==MPU6500_ID) //器件ID正确
    {
        MPU_Write_Byte(MPU9250_ADDR,MPU_PWR_MGMT1_REG,0X01);    //设置CLKSEL,PLL X轴为参考
        MPU_Write_Byte(MPU9250_ADDR,MPU_PWR_MGMT2_REG,0X00);    //加速度与陀螺仪都工作
        MPU_Set_Rate(50);                               //设置采样率为50Hz
    }else return 1;

    res=MPU_Read_Byte(AK8963_ADDR,MAG_WIA);             //读取AK8963 ID
    if(res==AK8963_ID)
    {
        MPU_Write_Byte(AK8963_ADDR,MAG_CNTL1,0X11);     //设置AK8963为单次测量模式
    }else return 1;

    return 0;
}

//设置MPU9250陀螺仪传感器满量程范围
//fsr:0,±250dps;1,±500dps;2,±1000dps;3,±2000dps
//返回值:0,设置成功
//    其他,设置失败
u8 MPU_Set_Gyro_Fsr(u8 fsr)
{
    return MPU_Write_Byte(MPU9250_ADDR,MPU_GYRO_CFG_REG,fsr<<3);//设置陀螺仪满量程范围
}
//设置MPU9250加速度传感器满量程范围
//fsr:0,±2g;1,±4g;2,±8g;3,±16g
//返回值:0,设置成功
//    其他,设置失败
u8 MPU_Set_Accel_Fsr(u8 fsr)
{
    return MPU_Write_Byte(MPU9250_ADDR,MPU_ACCEL_CFG_REG,fsr<<3);//设置加速度传感器满量程范围
}

//设置MPU9250的数字低通滤波器
//lpf:数字低通滤波频率(Hz)
//返回值:0,设置成功
//    其他,设置失败
u8 MPU_Set_LPF(u16 lpf)
{
    u8 data=0;
    if(lpf>=188)data=1;
    else if(lpf>=98)data=2;
    else if(lpf>=42)data=3;
    else if(lpf>=20)data=4;
    else if(lpf>=10)data=5;
    else data=6;
    return MPU_Write_Byte(MPU9250_ADDR,MPU_CFG_REG,data);//设置数字低通滤波器
}

//设置MPU9250的采样率(假定Fs=1KHz)
//rate:4~1000(Hz)
//返回值:0,设置成功
//    其他,设置失败
u8 MPU_Set_Rate(u16 rate)
{
    u8 data;
    if(rate>1000)rate=1000;
    if(rate<4)rate=4;
    data=1000/rate-1;
    data=MPU_Write_Byte(MPU9250_ADDR,MPU_SAMPLE_RATE_REG,data); //设置数字低通滤波器
    return MPU_Set_LPF(rate/2); //自动设置LPF为采样率的一半
}

//得到温度值
//返回值:温度值(扩大了100倍)
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
//得到陀螺仪值(原始值)
//gx,gy,gz:陀螺仪x,y,z轴的原始读数(带符号)
//返回值:0,成功
//    其他,错误代码
short Deviation_gyro[3],Original_gyro[3];
int Deviation_Count;
short gyro[3];
u8 MPU_Get_Gyroscope(short *gx,short *gy,short *gz)
{
    u8 buf[6],res;
    res=MPU_Read_Len(MPU9250_ADDR,MPU_GYRO_XOUTH_REG,6,buf);
    if(res==0)
    {
        if(Deviation_Count<CONTROL_DELAY)//开机前，读取陀螺仪零点
        {
            Deviation_Count++;
            memcpy(Deviation_gyro,gyro,sizeof(gyro));
            // 读10秒的原始数据
            gyro[0] = ((u16)buf[0]<<8)|buf[1];
            gyro[1] = ((u16)buf[2]<<8)|buf[3];
            gyro[2] = ((u16)buf[4]<<8)|buf[5];
        }else{
            // 读出来的是原始数据
            Original_gyro[0] = ((u16)buf[0]<<8)|buf[1];
            Original_gyro[1] = ((u16)buf[2]<<8)|buf[3];
            Original_gyro[2] = ((u16)buf[4]<<8)|buf[5];
            // 原始数据减去静止时的偏移量
            gyro[0] =Original_gyro[0]-Deviation_gyro[0];  //最终带去除零点漂移的数据
            gyro[1] =Original_gyro[1]-Deviation_gyro[1];
            gyro[2]= Original_gyro[2]-Deviation_gyro[2];
        }
        // 将最终的数据返回出去
        *gx=gyro[0];
        *gy=gyro[1];
        *gz=gyro[2];
    }
    return res;;
}
//得到加速度值(原始值)
//gx,gy,gz:陀螺仪x,y,z轴的原始读数(带符号)
//返回值:0,成功
//    其他,错误代码
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

//得到磁力计值(原始值)
//mx,my,mz:磁力计x,y,z轴的原始读数(带符号)
//返回值:0,成功
//    其他,错误代码
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
    MPU_Write_Byte(AK8963_ADDR,MAG_CNTL1,0X11); //AK8963每次读完以后都需要重新设置为单次测量模式
    return res;;
}

//IIC连续写
//addr:器件地址
//reg:寄存器地址
//len:写入长度
//buf:数据区
//返回值:0,正常
//    其他,错误代码
u8 MPU_Write_Len(u8 addr,u8 reg,u8 len,u8 *buf)
{
    HAL_I2C_Mem_Write(&hi2c1,addr<<1,reg,I2C_MEMADD_SIZE_8BIT,buf,len,300);
    /* u8 i;
     IIC_Start();
     IIC_Send_Byte((addr<<1)|0); //发送器件地址+写命令
     if(IIC_Wait_Ack())          //等待应答
     {
         IIC_Stop();
         return 1;
     }
     IIC_Send_Byte(reg);         //写寄存器地址
     IIC_Wait_Ack();             //等待应答
     for(i=0;i<len;i++)
     {
         IIC_Send_Byte(buf[i]);  //发送数据
         if(IIC_Wait_Ack())      //等待ACK
         {
             IIC_Stop();
             return 1;
         }
     }
     IIC_Stop();*/
    return 0;
}

//IIC连续读
//addr:器件地址
//reg:要读取的寄存器地址
//len:要读取的长度
//buf:读取到的数据存储区
//返回值:0,正常
//    其他,错误代码
u8 MPU_Read_Len(u8 addr,u8 reg,u8 len,u8 *buf)
{
    HAL_I2C_Mem_Read(&hi2c1, addr<<1, reg, I2C_MEMADD_SIZE_8BIT, buf, len, 300);
    /*IIC_Start();
    IIC_Send_Byte((addr<<1)|0); //发送器件地址+写命令
    if(IIC_Wait_Ack())          //等待应答
    {
        IIC_Stop();
        return 1;
    }
    IIC_Send_Byte(reg);         //写寄存器地址
    IIC_Wait_Ack();             //等待应答
    IIC_Start();
    IIC_Send_Byte((addr<<1)|1); //发送器件地址+读命令
    IIC_Wait_Ack();             //等待应答
    while(len)
    {
        if(len==1)*buf=IIC_Read_Byte(0);//读数据,发送nACK
        else *buf=IIC_Read_Byte(1);     //读数据,发送ACK
        len--;
        buf++;
    }
    IIC_Stop();                 //产生一个停止条件*/
    return 0;
}

//IIC写一个字节
//devaddr:器件IIC地址
//reg:寄存器地址
//data:数据
//返回值:0,正常
//    其他,错误代码
u8 MPU_Write_Byte(u8 addr,u8 reg,u8 data)
{
    HAL_I2C_Mem_Write(&hi2c1,addr<<1,reg,I2C_MEMADD_SIZE_8BIT,&data,1,300);
//    IIC_Start();
////    IIC_Send_Byte((addr<<1)|0); //发送器件地址+写命令
////    if(IIC_Wait_Ack())          //等待应答
////    {
////        IIC_Stop();
////        return 1;
////    }
////    IIC_Send_Byte(reg);         //写寄存器地址
////    IIC_Wait_Ack();             //等待应答
////    IIC_Send_Byte(data);        //发送数据
////    if(IIC_Wait_Ack())          //等待ACK
////    {
////        IIC_Stop();
////        return 1;
////    }
////    IIC_Stop();
    return 0;
}

//IIC读一个字节
//reg:寄存器地址
//返回值:读到的数据
u8 MPU_Read_Byte(u8 addr,u8 reg)
{
    uint8_t res;
    HAL_I2C_Mem_Read(&hi2c1, addr<<1, reg, I2C_MEMADD_SIZE_8BIT, &res, 1, 300);
//    u8 res;
//    IIC_Start();
//    IIC_Send_Byte((addr<<1)|0); //发送器件地址+写命令
//    IIC_Wait_Ack();             //等待应答
//    IIC_Send_Byte(reg);         //写寄存器地址
//    IIC_Wait_Ack();             //等待应答
//    IIC_Start();
//    IIC_Send_Byte((addr<<1)|1); //发送器件地址+读命令
//    IIC_Wait_Ack();             //等待应答
//    res=IIC_Read_Byte(0);     //读数据,发送nACK
//    IIC_Stop();                 //产生一个停止条件

    return res;
}