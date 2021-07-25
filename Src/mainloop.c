#include "mainloop.h"

extern ADC_HandleTypeDef hadc1;

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
//extern TIM_HandleTypeDef htim5;
//extern TIM_HandleTypeDef htim8;

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart3;
extern DMA_HandleTypeDef hdma_usart1_rx;
extern DMA_HandleTypeDef hdma_usart1_tx;
extern bool bEmergencyStop;

uint8_t pucSoftwareVersion[3]={SOFTWARE_MAIN_VERSION,SOFTWARE_SUBVERSION,SOFTWARE_PATCHLEVEL};
uint8_t pucHardwareVersion[3]={HARDWARE_MAIN_VERSION,HARDWARE_SUBVERSION,HARDWARE_PATCHLEVEL};

volatile uint8_t ucUart1RxBufLen = 0;
volatile uint8_t ucUartRecvEndCounter = 0;
uint8_t pucUart1RxBuf[UART_RX_BUF_SIZE] = {0};
uint8_t pucUart1RxBufCache[UART_RX_BUF_SIZE] = {0};

uint8_t pucUart1TxBuf[UART_TX_BUF_SIZE] = {0};
uint8_t pucUart1TxBufCache[UART_TX_BUF_SIZE] = {0};
volatile uint16_t usUart1TxCacheCounter = 0;
volatile uint8_t ucUart1TxDMAOverFlag = 1;

float pfGyroZBuf[6] = {0.0};
float fGyroZ = 0.0;


uint8_t ucCommandSendOverFlag = 1;
uint8_t ucCommandSendErrorFlag = 0;
uint8_t bMpuIntCounter = 0;
Motor_t xMotor1,xMotor2;
#if BASE_TYPE == BASE_4WD
Motor_t xMotor3,xMotor4;
#endif
#if BASE_TYPE == BASE_ACKERMAN
Servo_t xServo;
#endif
uint32_t puiUID[3] = {0};

uint32_t ucTxCounter,ucRxCounter;
extern volatile uint32_t uiLastVelCmdTick;
TIM_HandleTypeDef* usTimer;
static bool bTestMode = false;


int fputc(int ch, FILE *f)
{    
	HAL_UART_Transmit(&huart3,(uint8_t*)&ch,1,10); 
	return ch;
}
/* Debug opts */
#define LOG_LEVEL  1
#if LOG_LEVEL == 0

#else
void Errorprint(const char* format, ...);
void Warningprint(const char* format, ...);
void Debugprint(const char* format, ...);
void Infoprint(const char* format, ...);
#endif


void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart){
	if(huart->Instance == USART1){
		ucCommandSendErrorFlag = 1;
	}
}
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart){
	if(huart->Instance == USART1){
		HAL_UART_DMAStop(&huart1);
		ucCommandSendOverFlag = 1;
	}
}
	
void vSendCommand(uint8_t* pucSendBuf,uint8_t ucSendBufLen){
	if(usUart1TxCacheCounter + ucSendBufLen < UART_TX_BUF_SIZE){
		memcpy(pucUart1TxBufCache+usUart1TxCacheCounter,pucSendBuf,ucSendBufLen);
		usUart1TxCacheCounter += ucSendBufLen;
	}
	else{
		Errorprint("TX Cache is Full %d %d %d %d",ucSendBufLen,usUart1TxCacheCounter,ucUart1TxDMAOverFlag,__HAL_DMA_GET_COUNTER(&hdma_usart1_tx));
	}
}
void vCommandLoopSend(void){
	if((usUart1TxCacheCounter>0) && (ucUart1TxDMAOverFlag)){
		memcpy(pucUart1TxBuf,pucUart1TxBufCache,usUart1TxCacheCounter);
		if(HAL_OK != HAL_UART_Transmit_DMA(&huart1,pucUart1TxBuf,usUart1TxCacheCounter)){
			printf("DMA Faild %d \r\n",HAL_UART_Transmit_DMA(&huart1,pucUart1TxBuf,usUart1TxCacheCounter));
		}
		usUart1TxCacheCounter = 0;
		ucUart1TxDMAOverFlag = 0;
	}
}


void vMotorHWInit(void){
	HAL_GPIO_WritePin(MOTOR_STBY_GPIO_Port,MOTOR_STBY_Pin,GPIO_PIN_SET);
	
	HAL_GPIO_WritePin(MOTOR1_AN1_GPIO_Port,MOTOR1_AN1_Pin,GPIO_PIN_SET);
	HAL_GPIO_WritePin(MOTOR2_AN1_GPIO_Port,MOTOR2_AN1_Pin,GPIO_PIN_SET);
#if BASE_TYPE == BASE_4WD
	HAL_GPIO_WritePin(MOTOR3_AN1_GPIO_Port,MOTOR3_AN1_Pin,GPIO_PIN_SET);
	HAL_GPIO_WritePin(MOTOR4_AN1_GPIO_Port,MOTOR4_AN1_Pin,GPIO_PIN_SET);
#endif
	
	HAL_GPIO_WritePin(MOTOR1_AN2_GPIO_Port,MOTOR1_AN2_Pin,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(MOTOR2_AN2_GPIO_Port,MOTOR2_AN2_Pin,GPIO_PIN_RESET);
#if BASE_TYPE == BASE_4WD
	HAL_GPIO_WritePin(MOTOR3_AN2_GPIO_Port,MOTOR3_AN2_Pin,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(MOTOR4_AN2_GPIO_Port,MOTOR4_AN2_Pin,GPIO_PIN_RESET);
#endif
	
	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_2);
	#if BASE_TYPE == BASE_4WD
	HAL_TIM_PWM_Start(&htim5,TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim5,TIM_CHANNEL_4);
#endif

	__HAL_TIM_SET_COUNTER(&htim4,0);
	__HAL_TIM_SET_COUNTER(&htim3,0);

	HAL_TIM_Encoder_Start(&htim3,TIM_CHANNEL_ALL);
	HAL_TIM_Encoder_Start(&htim4,TIM_CHANNEL_ALL);
			
			
	vMotorInit(&xMotor1,MOTOR1,ENCODER1);
	vMotorInit(&xMotor2,MOTOR2,ENCODER2);
	#if BASE_TYPE == BASE_4WD
	vMotorInit(&xMotor3,MOTOR3,ENCODER3);
	vMotorInit(&xMotor4,MOTOR4,ENCODER4);	
	#endif
	
	#if BASE_TYPE == BASE_ACKERMAN
		HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_1);
		HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_2);
	#endif
}
static uint16_t  usGetADC1Value(uint32_t ch){
	HAL_StatusTypeDef xHalStatus = HAL_OK;
	ADC_ChannelConfTypeDef ADC1_ChanConf;
	ADC1_ChanConf.Channel=ch;                                 //通道
	ADC1_ChanConf.Rank=1;                                     //第1个序列，序列1
	ADC1_ChanConf.SamplingTime = ADC_SAMPLETIME_55CYCLES_5;   //采样时间     
	HAL_ADC_ConfigChannel(&hadc1,&ADC1_ChanConf);        			//通道配置
	HAL_ADC_Start(&hadc1);                               			//开启ADC
	xHalStatus = HAL_ADC_PollForConversion(&hadc1,10);      	//轮询转换
	if(xHalStatus != HAL_OK)
		printf("usGetADC1Value Faild\r\n");
	return (uint16_t)HAL_ADC_GetValue(&hadc1);	        			//返回最近一次ADC1规则组的转换结果
}  
/**
return value:
1 : imu error
4 : bat senser error
10 : Motor or encoder error
*/
static uint8_t ucSelfTest(uint8_t ucTestMode){
	static uint8_t ucTastItem = 0;
	static uint32_t uiTestTick = 0;
	static uint16_t usTestGap = 0;
	static int32_t iDeltaEncoder[2];
	if(HAL_GetTick() - uiTestTick > usTestGap){
		switch (ucTastItem){
			case 0:{
				uiTestTick = HAL_GetTick();
				usTestGap = 10;
				ucTastItem++;
				Debugprint("ACCEL %f %f %f",xImu.pfAccel[0],xImu.pfAccel[1],xImu.pfAccel[2]);
				if(((xImu.pfAccel[0]==0.0)&&(xImu.pfAccel[1]==0.0))||(fabs(xImu.pfAccel[2]-10))>1){
					return ucTastItem;
				}
				break;
			}
			case 1:{
				uiTestTick = HAL_GetTick();
				usTestGap = 10;
				ucTastItem++;			
				Debugprint("GYRO %f %f %f",xImu.pfGyro[0],xImu.pfGyro[1],xImu.pfGyro[2]);
				break;
			}
			case 2:{
				uiTestTick = HAL_GetTick();
				usTestGap = 10;
				ucTastItem++;			
				Debugprint("Euler %f %f %f",xImu.pfEuler[0],xImu.pfEuler[1],xImu.pfEuler[2]);
				break;
			}
			case 3:{
				uiTestTick = HAL_GetTick();
				usTestGap = 100;
				ucTastItem++;			
				Debugprint("BAT %f %f",xBattery.fVoltage ,xBattery.fCurrent);
				if((xBattery.fVoltage<7.0) || (xBattery.fVoltage > 13.0) || (xBattery.fCurrent < 0.01) || (xBattery.fCurrent > 3.0)){
					return ucTastItem;
				}
				break;
			}
			extern void vSetServoPwm(uint16_t usPwm);
			case 4:{
				uiTestTick = HAL_GetTick();
				usTestGap = 500;
				ucTastItem++;	
				vSetServoPwm(1300);
				break;				
			}				
			case 5:{
				uiTestTick = HAL_GetTick();
				usTestGap = 1000;
				ucTastItem++;	
				vSetServoPwm(1800);
				break;				
			}	
			case 6:{
				uiTestTick = HAL_GetTick();
				usTestGap = 500;
				ucTastItem++;	
				vSetServoPwm(1500);
				break;				
			}		
			case 7:{
				uiTestTick = HAL_GetTick();
				usTestGap = 1000;
				ucTastItem++;	
				Debugprint("CW Start Encoder:%d %d",xMotor1.iLastEncoderReadValue,xMotor2.iLastEncoderReadValue);
				iDeltaEncoder[0] = xMotor1.iLastEncoderReadValue;
				iDeltaEncoder[1] = xMotor2.iLastEncoderReadValue;			
				vSetMotorPWM(MOTOR1,300);
				vSetMotorPWM(MOTOR2,300);
				break;
			}			
			case 8:{
				uiTestTick = HAL_GetTick();
				usTestGap = 100;
				ucTastItem++;	
				vSetMotorPWM(MOTOR1,0);
				vSetMotorPWM(MOTOR2,0);
				Debugprint("CW End Encoder:%d %d",xMotor1.iLastEncoderReadValue,xMotor2.iLastEncoderReadValue);
				iDeltaEncoder[0] -= xMotor1.iLastEncoderReadValue;
				iDeltaEncoder[1] -= xMotor2.iLastEncoderReadValue;
				break;				
			}	
			case 9:{
				uiTestTick = HAL_GetTick();
				usTestGap = 100;
				ucTastItem++;	
				Debugprint("CW Delta Encoder:%d %d",iDeltaEncoder[0],iDeltaEncoder[1]);
				if((iDeltaEncoder[0] > 0) && (iDeltaEncoder[1] > 0)){ //Both two encoder equip
					static float fEncoderScale;
					fEncoderScale = iDeltaEncoder[0]/iDeltaEncoder[1];
					if((fEncoderScale>1.1) || (fEncoderScale<0.9)){
						return ucTastItem;
					}
				}
				else if((iDeltaEncoder[0]+iDeltaEncoder[1]!=0)&&(iDeltaEncoder[0]*iDeltaEncoder[1]==0)){ //only one encoder equip and another missed
					return ucTastItem;
				}
				else; //Both two encoder NOT equip,we allow this case in board test mode
				break;				
			}			
			case 10:{
				uiTestTick = HAL_GetTick();
				usTestGap = 1000;
				ucTastItem++;	
				Debugprint("CCW Start Encoder:%d %d",xMotor1.iLastEncoderReadValue,xMotor2.iLastEncoderReadValue);
				iDeltaEncoder[0] = xMotor1.iLastEncoderReadValue;
				iDeltaEncoder[1] = xMotor2.iLastEncoderReadValue;			
				vSetMotorPWM(MOTOR1,-300);
				vSetMotorPWM(MOTOR2,-300);
				break;
			}			
			case 11:{
				uiTestTick = HAL_GetTick();
				usTestGap = 100;
				ucTastItem++;	
				vSetMotorPWM(MOTOR1,0);
				vSetMotorPWM(MOTOR2,0);
				Debugprint("CCW End Encoder:%d %d",xMotor1.iLastEncoderReadValue,xMotor2.iLastEncoderReadValue);
				iDeltaEncoder[0] = xMotor1.iLastEncoderReadValue - iDeltaEncoder[0];
				iDeltaEncoder[1] = xMotor2.iLastEncoderReadValue - iDeltaEncoder[1];
				break;				
			}	
			case 12:{
				uiTestTick = HAL_GetTick();
				usTestGap = 100;
				ucTastItem++;	
				Debugprint("CCW Delta Encoder:%d %d",iDeltaEncoder[0],iDeltaEncoder[1]);
				if((iDeltaEncoder[0] > 0) && (iDeltaEncoder[1] > 0)){ //Both two encoder equip
					static float fEncoderScale;
					fEncoderScale = iDeltaEncoder[0]/iDeltaEncoder[1];
					if((fEncoderScale>1.1) || (fEncoderScale<0.9)){
						return 10;
					}
				}
				else if((iDeltaEncoder[0]+iDeltaEncoder[1]!=0)&&(iDeltaEncoder[0]*iDeltaEncoder[1]==0)){ //only one encoder equip and another missed
					return 10;
				}
				else; 	//Both two encoder NOT equip,we allow this case in board test mode
				break;				
			}					
			default:{
				bTestMode = false;
				ucTastItem = 0;
				usTestGap = 0;
				break;
			}
		}	
	}
	return 0;
}
/**
Main Loop Function
*/
void vMainLoop(void){
	uint32_t ucImuLastReadTick = 0;
	uint8_t ucTakeCacheResult = 0;
	uint32_t uiMotorLastContorTick = 0;
	uint32_t uiBatteryVoltageLastCheckTick = 0;
	uint32_t uiBatteryCurrentLastCheckTick = 0;	
	uint32_t uiPS2LastCheckTick = 0;
	uint32_t uiLastRecvProtocolTick = 0;
	#if SONAR_CHANNEL_NUM > 0
	uint32_t uiSonsrCheckTick = 0;
	#endif	
	uint32_t uiKeyOneCounter = 0;
	bool bImuReadyFlag = false;
	bool bUartConnectionFlag = false;
	bool bUartLostConnectionMark = false;
	bool bUartEstablishConnectionMark = false;
	bool bBatteryLowFlag = false;
	bool bBatteryStateChangeFlag = false;
	bool bPS2OperateFlag = false;
	uint16_t usVersionADC = 0;
	//hardware version detection
	usVersionADC = usGetADC1Value(ADC_CHANNEL_5);
	if(usVersionADC < 20)																	//Pin Set Grount,0 Volt
		pucHardwareVersion[1] = 1; 
	else if ((usVersionADC < 446) && usVersionADC > 297)	//Pin Set 1K/10K Resistance
		pucHardwareVersion[1] = 2;
	else if ((usVersionADC < 634) && usVersionADC > 445)	//Pin Set 1.5K/10K Resistance
		pucHardwareVersion[1] = 3;
	else 
		pucHardwareVersion[1] = 0;
	//Uart3 as debug print port,Disable Uart3 IRQ to avoid unexpect problem
	HAL_NVIC_DisableIRQ(USART3_IRQn);
	
	#if BASE_TYPE == BASE_ACKERMAN
		HAL_GPIO_WritePin(LED_BLUE_GPIO_Port,LED_BLUE_Pin,GPIO_PIN_SET);
		bool bGyroStable = false;
		if(HAL_GPIO_ReadPin(SWITCH1_GPIO_Port,SWITCH1_Pin) == GPIO_PIN_SET){
			bGyroStable = true;
		}	
	#else
		HAL_GPIO_WritePin(LED_GREEN_GPIO_Port,LED_GREEN_Pin,GPIO_PIN_SET);
	#endif
	vIndicateInit();
	vInitCommandCache();
	vPidInit();
	vMotorHWInit();
	vPS2Init();
	HAL_GetUID(puiUID);
	HAL_Delay(1000);
	/*init IMU module*/
	uint8_t ucTemp = 0;
	uint8_t ucImuErrorCode = 0;
	
	while(bImuReadyFlag == false){
		ucImuErrorCode = ucImuInit();
		if(ucImuErrorCode!=0){
			printf("Err code %d\r\n",ucImuErrorCode);
			ucTemp++;
			HAL_Delay(20);
			if(ucTemp > IMU_CHECK_MAX_TRY){
				Errorprint("IMU Faild");			
				break;
			}
		}
		else{
			bImuReadyFlag = true;
			bMpuIntCounter = 0;
		}
	}
	/*set indicate state*/
	if(bImuReadyFlag){	/*buzzer beep in 1Hz and keep beep*/
		vSetIndicate(LEDG,0.5,0);	/*work well indicate*/
	}
	else{
		if(ucImuErrorCode <12){
			vSetIndicate(BEEP,1,0);
			vSetIndicate(LEDR,0.5,0);		
		}
		else{
			vSetIndicate(BEEP,1,0);
			vSetIndicate(LEDB,0.5,0);
			printf("Error Code %d\r\n",ucImuErrorCode);			
		}

	}
	/*clear uart1 IDLE flag and enable IDLE IRQ*/
	__HAL_UART_CLEAR_IDLEFLAG(&huart1);
	__HAL_UART_ENABLE_IT(&huart1,UART_IT_IDLE);
	__HAL_UART_CLEAR_IDLEFLAG(&huart1);

	ucUartRecvEndCounter = 0;
	ucUart1RxBufLen = 0;
	/*Set uart1 DAM receive*/
	HAL_UART_Receive_DMA(&huart1,pucUart1RxBuf,UART_RX_BUF_SIZE);
	
	#if BASE_TYPE == BASE_ACKERMAN
	vServoStart(&xServo);	
	#endif

	bMpuIntCounter = 0;
	/*Set Motor accelerated speed,unit is m/ss*/
	vSetMotorAccelDecel(usConvertAcc2MotorAcc(5.5));
	
	while(1){
		/*establish connection*/
		if(bUartEstablishConnectionMark){
			vSetIndicate(LEDG,2,0);
			/*init IMU to set data to zero when first establish connection*/
			if(bImuReadyFlag){
				ucImuInit();
				bMpuIntCounter = 0;
			}
			vSetIndicate(BEEP,0.5,4000);/*TODO:need beep 2S here,this methode  may not a good idea*/
			bUartConnectionFlag = true;
			bUartEstablishConnectionMark = false;
		}
		/*connection lost*/
		if(bUartLostConnectionMark){
			vSetIndicate(LEDG,0.5,0);
			vSetIndicate(BEEP,2,3000);
			bUartConnectionFlag = false;
			bUartLostConnectionMark = false;
		}
		/*battery low*/
		if(bBatteryStateChangeFlag){
			bBatteryStateChangeFlag = false;
			if(bBatteryLowFlag){
				vSetIndicate(LEDB,2,0);
				vSetIndicate(LEDG,0,0);
				vSetIndicate(BEEP,0.5,0);
			}
			else{
				vSetIndicate(LEDB,0,0);
				vSetIndicate(BEEP,0,0);				
				if(bImuReadyFlag){
					if(bUartConnectionFlag){
						vSetIndicate(LEDG,2,0);
					}
					else{
						vSetIndicate(LEDG,0.5,0);
					}
				}
				else{
					vSetIndicate(BEEP,1,0);
					vSetIndicate(LEDR,0.5,0);
				}		
			}
		}

		/*uart received data*/
		if(ucUartRecvEndCounter != 0){
			ucUartRecvEndCounter = 0;
			ucRxCounter+=ucUart1RxBufLen;
			ucSaveCommandToCache(ucUart1RxBufLen,pucUart1RxBufCache);
			memset(pucUart1RxBufCache,0,UART_RX_BUF_SIZE);
			ucUart1RxBufLen = 0;
		}
		vCommandLoopSend();
		/*take data from cache*/
		ucTakeCacheResult = usTakeCommandFromCache();
		while(ucTakeCacheResult==0){	/*take cache until cache is empty*/
			if(bUartConnectionFlag == false){	/*change connection mark when connect state form disconn switch to connect*/
				bUartEstablishConnectionMark = true;
			}
			uiLastRecvProtocolTick = HAL_GetTick();
			ucCommandInterpretation(ucCommandTempLen,pucCommandTempBuf);
			ucTakeCacheResult = usTakeCommandFromCache();
			if(HAL_GetTick() - ucImuLastReadTick > (1000/DEFAULT_MPU_HZ))	/*loop max time,over this time may miss a MPU data*/
				break;
		}
		/* */
		if(ucTakeCacheResult > 1){
			Errorprint("Take Cache Error %d %d %d %d ",ucTakeCacheResult,uiGetCacheCounter,uiTakeCacheCounter,HAL_GetTick());
		}
		else
			;
		/*get IMU INT signal,take data from IMU*/
		if(bMpuIntCounter){
			if(bMpuIntCounter != 1){
				Warningprint("Miss %d MPU INT",bMpuIntCounter-1);
			}
			if(ucGetImuDataGroup(&xImu)){
				Errorprint("Read IMU Faild %d ",bMpuIntCounter);
			}
			else{
				fGyroZ = 0.0;
				pfGyroZBuf[5] = xImu.pfGyro[2];
				for(uint8_t i = 0;i < 5;i++){
					pfGyroZBuf[i] = pfGyroZBuf[i+1];
					fGyroZ += pfGyroZBuf[i];
				}
				fGyroZ /= 5;
				
			}
			ucImuLastReadTick = HAL_GetTick();
			bMpuIntCounter = 0;		
		}
		/*motor control*/
		if((HAL_GetTick() - uiMotorLastContorTick > MOTOR_CONTROL_PERIOD)){	/*Motor Control*/
			vGetMotorSpeed(&xMotor1);
			vGetMotorSpeed(&xMotor2);
			#if BASE_TYPE == BASE_4WD
			vGetMotorSpeed(&xMotor3);
			vGetMotorSpeed(&xMotor4);
			#endif
			if(!bTestMode){
				vExecuteMotorSpeed(&xMotor1,fMotorPidCompute(&xMotor1));
				vExecuteMotorSpeed(&xMotor2,fMotorPidCompute(&xMotor2));
				#if BASE_TYPE == BASE_4WD
				vExecuteMotorSpeed(&xMotor3,fMotorPidCompute(&xMotor3));
				vExecuteMotorSpeed(&xMotor4,fMotorPidCompute(&xMotor4));
				#endif
				vGetSpeed(&xVelocity);
				#if BASE_TYPE == BASE_ACKERMAN
				xServo.fCurrentAngle = CONVERT_OMEGA2ANGLE((xVelocity.fAngleZ),(xVelocity.fLineX));
				xServo.fVelocity = xVelocity.fLineX;
				vSetServoAngle(&xServo,bGyroStable);
				#endif			
			}

			uiMotorLastContorTick = HAL_GetTick();			
		}
		/*battery current check*/
		if(HAL_GetTick() - uiBatteryCurrentLastCheckTick > BATTERY_CURRENT_CHECK_PERIOD){	/*Battery Current Check*/
			uiBatteryCurrentLastCheckTick = HAL_GetTick();
			static uint16_t usTempCounter = 0;
			vGetBatteryCurrentInfo(&xBattery);
			usTempCounter++;
			if(usTempCounter>500){
				usTempCounter = 0;
			}
		}
		/*battery voltage check*/
		if(HAL_GetTick() - uiBatteryVoltageLastCheckTick > BATTERY_VOLTAGE_CHECK_PERIOD){	/*Battery Voltage Check*/
			uiBatteryVoltageLastCheckTick = HAL_GetTick();

			vGetBatteryVoltageInfo(&xBattery);
			#ifdef DEBUG_MODE 	//in debug mode,ignore BATTERY_LOW.
			static uint8_t prvucCounter = 0;
			if(xBattery.fCurrent > 1.2){
				if(bBatteryLowFlag==false)
					prvucCounter++;
			}
			else{
				if(prvucCounter>0){
					prvucCounter = 0;
				}
				if(bBatteryLowFlag){
					if(xBattery.fCurrent < 0.1){
						bBatteryStateChangeFlag = true;
						bBatteryLowFlag  =false;
					}
				}
			}
			if(prvucCounter > 3){
				bBatteryStateChangeFlag = true;
				bBatteryLowFlag = true;
			}			
			#else
			static uint8_t prvucCounter = 0;
			if(xBattery.fVoltage < BATTERY_LOW_VOLTAGE){
				if(bBatteryLowFlag==false)
					prvucCounter++;
			}
			else{
				if(prvucCounter>0){
					prvucCounter = 0;
				}
				if(bBatteryLowFlag){
					if(xBattery.fVoltage > BATTERY_LOW_VOLTAGE + VOLTAGE_STATE_CHANGE_GAP){
						bBatteryStateChangeFlag = true;
						bBatteryLowFlag  =false;
					}
				}
			}
			if(prvucCounter > 3){
				bBatteryStateChangeFlag = true;
				bBatteryLowFlag = true;
			}
			#endif
		}	
		/*Sonar data check */
		#if SONAR_CHANNEL_NUM > 0
		if((HAL_GetTick()-uiSonsrCheckTick > SONAR_CHECK_PERIOD) && bUartConnectionFlag){
				if(ucSonarGroupNum == 1){
					vTrigSonar(2);
				}
				else{
					vTrigSonar(1);
				}
				uiSonsrCheckTick  = HAL_GetTick();
		}	
		#endif	
		/*execute indicat device*/
		vIndicate();	/*Indicate,include RGB-LED,BUZZER*/
		/* check last communication time*/
		if((HAL_GetTick()-uiLastRecvProtocolTick>DISCONNECT_WATI_TIME)&&bUartConnectionFlag){
			bUartLostConnectionMark = true;
		}
		
		if(HAL_GetTick() - uiPS2LastCheckTick > PS2_CHECK_PERIOID){
			uiPS2LastCheckTick = HAL_GetTick();
			bPS2OperateFlag = bReadPS2Data();
		}		
				
		if(HAL_GetTick() - uiLastVelCmdTick > VELOCITY_LAST_TIME  && bPS2OperateFlag == false){
			vComputeMotorSpeed(0.0,0.0,0.0);
		}		

		/* check key press Action*/
		static uint8_t ucPressCounter = 0;
		static bool bKeyRelease = true;

		if(HAL_GPIO_ReadPin(KEY_1_GPIO_Port,KEY_1_Pin) == GPIO_PIN_RESET){
			if(bKeyRelease){
				uiKeyOneCounter = HAL_GetTick();
				ucPressCounter++;
				bKeyRelease = false;
			}
		}
		else{
			if(HAL_GetTick() - uiKeyOneCounter > 300 && uiKeyOneCounter){
				switch (ucPressCounter){
					case 1:
						Debugprint("Single Click\r\n");
						break;
					case 2:
						Debugprint("Double Click\r\n");
						break;
					case 3:
						Debugprint("Triple Click\r\n");
						bTestMode = true;
						vSetIndicate(BEEP,10,200);
						break;
					default:
						Debugprint("Unknow Click %d \r\n",ucPressCounter);
						break;
				}
				ucPressCounter = 0;
				uiKeyOneCounter = 0;
				bKeyRelease = true;
			}
			bKeyRelease = true;
		}
//		if((HAL_GetTick() - uiKeyOneCounter> 1000) && (uiKeyOneCounter!=0)){ //Enter IAP Mode
//			HAL_GPIO_TogglePin(BEEP_GPIO_Port,BEEP_Pin);
//			HAL_Delay(100);
//			HAL_GPIO_TogglePin(BEEP_GPIO_Port,BEEP_Pin);
//			HAL_NVIC_SystemReset();		
//		}
		
	/*Test Mode*/
		if(bTestMode){
			static uint8_t ucTestResult = 0;
			ucTestResult = ucSelfTest(0);
			if(ucTestResult){
				Errorprint("Test Result %d",ucTestResult);
				vSetIndicate(BEEP,ucTestResult,0);
			}
		}
		/* print debug info */

		#ifdef DEBUG_MODE
		/* debug info output*/
		if(HAL_GetTick() - uiLastDebugTick > DEBUG_PERIOD){			
			static uint8_t ucTempi = 0;
			if(HAL_GetTick() - uiLastDebugTick > DEBUG_PERIOD){	
				switch(ucTempi++){
					case 0:
						Debugprint("Euler %f %f %f",xImu.pfEuler[0],xImu.pfEuler[1],xImu.pfEuler[2]);
						break;
					case 1:
						#if BASE_TYPE == BASE_4WD
						Debugprint("MOTOR %d %d %d %d",xMotor1.sCurrentRpm,xMotor2.sCurrentRpm,xMotor3.sCurrentRpm,xMotor4.sCurrentRpm);
						#elif BASE_TYPE == BASE_2WD
						Debugprint("MOTOR %d %d",xMotor1.sCurrentRpm,xMotor2.sCurrentRpm);	
						#else
						Debugprint("MOTOR %d %d,Servo %d %f %f %f",xMotor1.sCurrentRpm,xMotor2.sCurrentRpm,xServo.usPwm,xServo.fTargetAngle,xServo.fCurrentAngle,xServo.fOutPutAngle);			
						#endif
						break;
					case 2:
						#if BASE_TYPE == BASE_4WD
						Debugprint("MOTOR %d %d %d %d",sRpm2PWM(xMotor1.sTargetRpm),sRpm2PWM(xMotor2.sTargetRpm),sRpm2PWM(xMotor3.sTargetRpm),sRpm2PWM(xMotor4.sTargetRpm));
						#else
						Debugprint("MOTOR %d %d",sRpm2PWM(xMotor1.sTargetRpm),sRpm2PWM(xMotor2.sTargetRpm));				
						#endif
						break;
					case 3:
						Debugprint("GYRO %f %f %f",xImu.pfGyro[0],xImu.pfGyro[1],xImu.pfGyro[2]);
						break;
					case 4:
						Debugprint("ACCEL %f %f %f",xImu.pfAccel[0],xImu.pfAccel[1],xImu.pfAccel[2]);
						break;
					case 5:
						Debugprint("BAT %f %f",xBattery.fVoltage ,xBattery.fCurrent);
						break;
					default:
						ucTempi=0;
						printf("\r\n");
						break;
				}
				uiLastDebugTick = HAL_GetTick();
			}	
		}
		#endif
	}
}

