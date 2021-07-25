#include<stdio.h>
#include<stdarg.h>
#include "APP/LogModule.h"

#ifdef DMA_PRINT
char pcDmaPrintBuf[128]={0};
UART_HandleTypeDef *pxDmaUart=NULL;

void vDmaPrintfInit(UART_HandleTypeDef *pxSetUart){
	pxDmaUart = pxSetUart;
}
void dma_printf(const char *format, ...)
{
    unsigned int length;
    va_list args;
    va_start(args, format);
    length = vsnprintf((char *)pcDmaPrintBuf, sizeof(pcDmaPrintBuf), (char *)format, args);
    va_end(args);
    HAL_UART_Transmit_DMA(pxDmaUart, (unsigned char *)pcDmaPrintBuf, length);
}

__weak void Errorprint(const char* format, ...){
    unsigned int length;
    va_list args;
    va_start(args, format);
		sprintf(pcDmaPrintBuf,"Error:");
    length = vsnprintf((char *)pcDmaPrintBuf+5, sizeof(pcDmaPrintBuf), (char *)format, args);
    va_end(args);
		sprintf(pcDmaPrintBuf+length+6,"\r\n");
    HAL_UART_Transmit_DMA(pxDmaUart, (unsigned char *)pcDmaPrintBuf, length+6+2);
}
__weak void Debugprint(const char* format, ...){
    unsigned int length;
    va_list args;
    va_start(args, format);
		sprintf(pcDmaPrintBuf,"Debug:");
    length = vsnprintf((char *)pcDmaPrintBuf+6, sizeof(pcDmaPrintBuf), (char *)format, args);
    va_end(args);
		sprintf(pcDmaPrintBuf+length+6,"\r\n");
    HAL_UART_Transmit_DMA(pxDmaUart, (unsigned char *)pcDmaPrintBuf, length+6+2);
}
__weak void Warningprint(const char* format, ...){
    unsigned int length;
    va_list args;
    va_start(args, format);
		sprintf(pcDmaPrintBuf,"Warning:");
    length = vsnprintf((char *)pcDmaPrintBuf+8, sizeof(pcDmaPrintBuf), (char *)format, args);
    va_end(args);
		sprintf(pcDmaPrintBuf+length+8,"\r\n");
    HAL_UART_Transmit_DMA(pxDmaUart, (unsigned char *)pcDmaPrintBuf, length+8+2);
}
__weak void Inforint(const char* format, ...){
    unsigned int length;
    va_list args;
    va_start(args, format);
		sprintf(pcDmaPrintBuf,"Info");
    length = vsnprintf((char *)pcDmaPrintBuf+5, sizeof(pcDmaPrintBuf), (char *)format, args);
    va_end(args);
		sprintf(pcDmaPrintBuf+length+5,"\r\n");
    HAL_UART_Transmit_DMA(pxDmaUart, (unsigned char *)pcDmaPrintBuf, length+5+2);
}
#else

static void printf_wrapperV(const char* format, va_list args_list){
	vprintf(format, args_list);
}

void printf_wrapper(const char* format, ...){
	va_list marker;
	va_start(marker, format);
	printf_wrapperV(format, marker);
	va_end(marker);
}
/**

*/
__weak void Errorprint(const char* format, ...){
	printf("Error:");
	va_list marker;
	va_start(marker, format);
	printf_wrapperV(format, marker);
	va_end(marker);
	printf("\r\n");
}
/**

*/
__weak void Debugprint(const char* format, ...){
	printf("Debug:");
	va_list marker;
	va_start(marker, format);
	printf_wrapperV(format, marker);
	va_end(marker);
	printf("\r\n");
}
/**

*/
__weak void Warningprint(const char* format, ...){
	printf("Warning:");
	va_list marker;
	va_start(marker, format);
	printf_wrapperV(format, marker);
	va_end(marker);
	printf("\r\n");
}
/**

*/
__weak void Infoprint(const char* format, ...){
	printf("Info:");
	va_list marker;
	va_start(marker, format);
	printf_wrapperV(format, marker);
	va_end(marker);
	printf("\r\n");
}

#endif
