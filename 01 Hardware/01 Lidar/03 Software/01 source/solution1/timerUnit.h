/*
 * timerUnit.h
 *
 * Created: 6/29/2023 1:41:01 PM
 *  Author: Thach
 */ 


#ifndef TIMERUNIT_H_
#define TIMERUNIT_H_

#include "./ATMegaXX09/ATMegaXX09Clock.h"
#include <avr/io.h>
#include <time.h>

#define NO_OF_STOPTIMER 5

//erste Moeglichkeit
typedef struct  
{
	uint32_t value:31;
	uint8_t lock:1;
}tickGenerator;

//zweite Moeglichkeit
typedef struct{
	uint8_t day:5;
	uint8_t hour:5;
	uint8_t min:6;
	uint32_t us;
}clockTimer_t;

typedef struct{
	TCA_t *adr;
	uint8_t initStatus:1;
	uint8_t running:1;
}timer_t;

typedef union
{
	uint8_t value;
	struct{
		uint8_t ENABLE:1;//Bit 0,1, low bit
		uint8_t CLKSEL:2;
		uint8_t :1;
		uint8_t :1;
		uint8_t :1;
		uint8_t :1;
		uint8_t :1;//Bit 7, high bit
	}Register;
}TCA0_CTRLA_t;




extern void timerInit(uint8_t resolution);
extern void wait(uint8_t us);

#endif /* TIMERUNIT_H_ */