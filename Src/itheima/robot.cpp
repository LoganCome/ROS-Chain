//
// Created by Kaijun on 2021/1/29.
//

#include <cstdio>
#include "robot.h"
#include "oled.h"
#include "Ackermann.h"
#include "mpu9250.h"
#include "protocol.h"
#include "common_uart.h"


// ����һ��С��
Ackermann car;

uint32_t lastOledRefreshTime;
void show_oled();

uint32_t lastPublishTime;
void publishIMUData();
/**
 * �����˳�ʼ��
 */
void HeimaRobotInit(){
    printf("heimarobot init success\r\n");
    common_uart_init();

    car.init();

    MPU9250_Init();
    OLED_Init();
//    car.servo->setRadian(-3.14/6);
    car.updateVel(0.2,3.14/6);


}
/**
 * ����������: ����ѭ��
 */
void HeimaRobotTick(){

    car.tick();

    // ���ⷢ��IMU������
    publishIMUData();
    // ��ʾoled
    show_oled();
}

void publishIMUData(){
    if(HAL_GetTick() - lastPublishTime < 50 ){
        return;
    }
    lastPublishTime = HAL_GetTick();

//    HAL_GPIO_TogglePin(GPIOE,GPIO_PIN_10);

    short temp = MPU_Get_Temperature();

    short ax,ay,az=0;
    MPU_Get_Accelerometer(&ax,&ay,&az);

    short gx,gy,gz=0;
    MPU_Get_Gyroscope(&gx,&gy,&gz);

    short mx,my,mz=0;
    MPU_Get_Magnetometer(&mx,&my,&mz);

    struct TXProtocol* protocol = new TXProtocol;
    uint8_t protocolLength = sizeof(TXProtocol);
    protocol->head0 = FLAG_HEAD0;
    protocol->head1 = FLAG_HEAD1;
    protocol->type = TYPE_UPLOAD;
    protocol->len = protocolLength;

    // �¶�
    protocol->temperature = temp;

    // ������ٶ�
    protocol->ax = ax;
    protocol->ay = ay;
    protocol->az = az;
    // ������ٶ�
    protocol->gx = gx;
    protocol->gy = gy;
    protocol->gz = gz;
    // ����ش�
    protocol->mx = mx;
    protocol->my = my;
    protocol->mz = mz;

    // ���ٶ�

    short V = (short)(car.getVel()*1000);
    protocol->velocity = V;
    //���ٶ�
    short W = (short)(car.getAngularVel()*1000);
    protocol->angular = W;
    // ֡β
    protocol->code = FLAG_TAIL;

    // ����λ����Ϣ���ͳ�ȥ
    common_uart_send((uint8_t*)protocol,protocolLength);

    delete protocol;
}

void common_uart_idle_callback(uint8_t receive_buf[],uint16_t receive_len){
    if(receive_len < 4){
        return;
    }
    //11 22 33 44 0xce, 0xfa, 0x02 10 0x00 0x00 0x00 0x00 0xad 0x00
    int i=0;
    while (i < receive_len){
        uint8_t head0 = receive_buf[i];
        if(head0 == FLAG_HEAD0){
            uint8_t head1 = receive_buf[i+1];
            if(head1 == FLAG_HEAD1){
                uint8_t data_type = receive_buf[i+2];
                if(data_type == 0x02){ // ������λ�����͹��������ٶȺͽ��ٶ�
                    uint8_t length = receive_buf[i+3];

                    if(receive_len < length ) return;

                    uint8_t vel_low = receive_buf[i+4];
                    uint8_t vel_high = receive_buf[i+5];
                    short vel = vel_high<<8|vel_low;
                    float V = (float)vel/1000;

                    uint8_t angular_low = receive_buf[i+6];
                    uint8_t angular_high = receive_buf[i+7];
                    short angular = angular_high<<8|angular_low;
                    float W = (float)angular/1000;

                    uint8_t code_low = receive_buf[i+8];
                    uint8_t code_high = receive_buf[i+9];
                    short code = code_high<<8|code_low;

                    if(code == FLAG_TAIL){
                        HAL_GPIO_TogglePin(GPIOE,GPIO_PIN_10);
                        //HAL_GPIO_TogglePin(GPIOE,GPIO_PIN_10);
                        car.updateVel(V,W);
                        break;
                    }
                }else if(data_type == 0x03){ // ��ǰδ�����Э������

                }

            }
        }

        i++;
    }





}


void show_oled(){
    if(HAL_GetTick() - lastOledRefreshTime  < 50 ){
        return;
    }
    lastOledRefreshTime = HAL_GetTick();

    uint8_t data[]="heimarobot";
    OLED_ShowString(0,0,data);

    OLED_ShowNumber(0,12,123456,6,12);

    OLED_Refresh_Gram();
}