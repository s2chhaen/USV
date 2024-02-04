/*
 * main.h
 *
 * Created: 10/26/2023 4:34:31 PM
 *  Author: Thach
 */ 


#ifndef MAIN_H_
#define MAIN_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include "Math/checksum.h"
#include "ATMegaXX09/ATMegaXX09Clock.h"
#include "ATMegaXX09/USART/USART.h"
#include "lidarHandler.h"
#include "timerUnit.h"
#include "lidarFilter.h"

#define F_CPU 20000000
#include <util/delay.h>

typedef struct {
	uint8_t dataRx:1;//
	uint8_t dataFilter:1;
	uint8_t dataTx:1;
}mainFlag_t;

#endif /* MAIN_H_ */