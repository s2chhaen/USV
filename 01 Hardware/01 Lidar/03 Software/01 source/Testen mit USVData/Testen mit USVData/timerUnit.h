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

typedef union{
	uint8_t value;
	struct{
		uint8_t ENABLE:1;//Bit 0, low bit
		uint8_t CLKSEL:3;
		uint8_t :1;
		uint8_t :1;
		uint8_t :1;//Bit 7, high bit
	}valueBitField;
}TCA0_CTRLA_t;

typedef union{
	uint8_t value;
	struct{
		uint8_t OVF:1;//Bit 0, low bit
		uint8_t :1;
		uint8_t :1;
		uint8_t :1;
		uint8_t CMP0:1;
		uint8_t CMP1:1;
		uint8_t CMP2:1;
		uint8_t :1;//Bit 7, high bit
	}valueBitField;
}TCA0_INTCTRL_t;

typedef union{
	uint8_t value;
	struct{
		uint8_t OVF:1;//Bit 0,1, low bit
		uint8_t :1;
		uint8_t :1;
		uint8_t :1;
		uint8_t CMP0:1;
		uint8_t CMP1:1;
		uint8_t CMP2:1;
		uint8_t :1;//Bit 7, high bit
	}valueBitField;
}TCA0_INTFLAGS_t;

typedef union{
	volatile uint16_t value;
	volatile struct{
		uint8_t lowByte:8;
		uint8_t highByte:8;
	}valueBitField;
}TCA0_CNT_t;

typedef union{
	volatile uint16_t value;
	volatile struct{
		uint8_t lowByte:8;
		uint8_t highByte:8;
	}valueBitField;
}TCA0_PER_t;

enum prescaler{
	DIV1 = 0x00,
	DIV2 = 0x01,
	DIV4 = 0x02,
	DIV8 = 0x03,
	DIV16 = 0x04,
	DIV64 = 0x05,
	DIV256 = 0x06,
	DIV1024 = 0x07
};

extern uint8_t timerInit(uint8_t rezConfig, uint8_t resolution);
extern void timer_setState(uint8_t state);
extern void timer_setCounter(int32_t value);
extern const int32_t timer_getCounter();
extern void timer_stopWatch(uint16_t val);

#endif /* TIMERUNIT_H_ */