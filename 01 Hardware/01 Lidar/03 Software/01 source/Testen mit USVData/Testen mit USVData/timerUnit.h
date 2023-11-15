/*
 * timerUnit.h
 *
 * Created: 6/29/2023 1:41:01 PM
 * Author: Thach
 * Version: 1.0
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

#define NO_OF_SUBTIMER 10
#define REZ_MODE_NO 3
#define CONVERT_FACTOR_S_2_MS 1000UL
#define CONVERT_FACTOR_S_2_US 1000000UL

//TODO Refaktorisierung im Prozess

enum timerResolution{
	REZ_US,
	REZ_MS,
	REZ_S
};

typedef struct{
	uint8_t init:1;
	uint8_t rez:2;
	uint8_t state:1;
}timerStatus_t;

extern uint8_t timerInit(uint8_t rezConfig, uint8_t resolution);
extern void timer_setState(uint8_t state);
extern void timer_setCounter(uint32_t value);
extern const int16_t timer_getCounter();
extern void timer_stopWatch(uint16_t val);

#endif /* TIMERUNIT_H_ */