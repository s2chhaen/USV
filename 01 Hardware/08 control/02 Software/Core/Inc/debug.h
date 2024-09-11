#ifndef INC_DEBUG_H_
#define INC_DEBUG_H_

#include <math.h>
#include <stdio.h>
#include "stm32g4xx_hal.h"

UART_HandleTypeDef handleDebugUart;

void initDebug(UART_HandleTypeDef);

void sendStrToUSB(char[], uint8_t );
void sendFloatToUSB(float);

#endif /* INC_DEBUG_H_ */
