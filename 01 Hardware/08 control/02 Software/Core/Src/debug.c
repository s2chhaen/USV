#include <math.h>
#include <stdio.h>
#include <string.h>
#include "debug.h"

void initDebug(UART_HandleTypeDef handle){
	handleDebugUart = handle;
}

void sendFloatToUSB(float fVal){
	uint8_t str[32];
	int strLen = snprintf(str,32,"%f\r",fVal);
	HAL_UART_Transmit(&handleDebugUart, &str, strLen, 100);
}

void sendStrToUSB(char* str, uint8_t len){
	HAL_UART_Transmit(&handleDebugUart, str, len, 100);
}
