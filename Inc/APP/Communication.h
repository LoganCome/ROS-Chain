#ifndef __APP_COMMUNICATION_H_
#define __APP_COMMUNICATION_H_
#include <stdint.h>

#define SEND_COMMAND_LENGTH	64

extern volatile uint8_t ucCommandTempLen;
extern uint8_t pucCommandTempBuf[32];

extern uint32_t uiGetCacheCounter;
extern uint32_t uiTakeCacheCounter;

void vSendCommand(uint8_t* pucSendBuf,uint8_t ucSendBufLen);
int8_t ucCommandInterpretation(uint8_t ucCommandLen,uint8_t* pucCommandBuf);
void vInitCommandCache(void);
uint8_t ucSaveCommandToCache(uint8_t ucCommandLen,uint8_t* pucCommandBuf);
uint8_t usTakeCommandFromCache(void);
unsigned char crc8_chk_value(unsigned char *message, unsigned char len);
#endif
