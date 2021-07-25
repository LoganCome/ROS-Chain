#ifndef __APP_LOGMODULE_H_
#define __APP_LOGMODULE_H_
#include "APP/config.h"

#ifdef DMA_PRINT
void vDmaPrintfInit(UART_HandleTypeDef *pxSetUart);
void dma_printf(const char *format, ...);
#endif	
void Errorprint(const char* format, ...);
void Debugprint(const char* format, ...);
void Warningprint(const char* format, ...);
void Infoprint(const char* format, ...);

#endif


