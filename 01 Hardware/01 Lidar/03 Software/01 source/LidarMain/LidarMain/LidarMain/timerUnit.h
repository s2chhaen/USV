/*
 * timerUnit.h
 *
 * Created: 6/29/2023 1:41:01 PM
 * Author: Thach
 * Version: 1.1
 * Revision: 1.0
 */ 


#ifndef TIMERUNIT_H_
#define TIMERUNIT_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include "./ATMegaXX09/ATMegaXX09Clock.h"
#include "ATMegaXX09/USART/USART.h"
#include "Math/MinMax.h"
#include "errorList.h"

/*Abkürzungen: Res/REZ = resolution
*/

#define CONVERT_FACTOR_S_2_MS 1000UL
#define CONVERT_FACTOR_S_2_US 1000000UL
#define TIMER_MAX_REZ_MS 3276
#define TIMER_MAX_REZ_S 3
#define TIMER_MIN_REZ_US 1000UL

enum timerResolution{
	REZ_US,
	REZ_MS,
	REZ_S
};

typedef struct{
	uint8_t init:1;
}timerStatus_t;

extern uint8_t timer_init(uint8_t rezConfig, uint16_t resUSV, uint16_t resLidar);

#endif /* TIMERUNIT_H_ */