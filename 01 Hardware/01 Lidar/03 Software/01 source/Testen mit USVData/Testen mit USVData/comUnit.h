/*
 * comUnit.h
 *
 * Created: 7/10/2023 4:48:20 AM
 * Author: Thach
 * Version: 1.0
 * Revision: 1.5
 */ 


#ifndef USERUNIT_H_
#define USERUNIT_H_

#include <stdint.h>
#include <avr/io.h>
#include <stdlib.h>
#include <string.h>
#include "errorList.h"
#include "ATMegaXX09/USART/USART.h"
#include "ATMegaXX09/FIFO/FIFO.h"
#include "Math/MinMax.h"
#include "timerUnit.h"

#define BYTE_RECEIVE_TIME_US 100
#define BUFFER_LEN 360

//Struktur für Konfiguration der USART-Einheit
typedef struct{
	uint8_t usartNo:2;
	uint32_t baudrate;
	uint8_t usartChSize;
	uint8_t parity;
	uint8_t stopbit;
	uint8_t sync:1;
	uint8_t mpcm:1;
	uint8_t address;
	uint8_t portMux:2;
}usartConfig_t;

typedef struct {
	volatile uint8_t data[BUFFER_LEN];
	volatile uint16_t strPtr;
	volatile uint16_t toHandleBytes;
	volatile uint8_t lock:1;
}comUnitBuffer_t;

typedef struct{
	uint8_t usart4USVData:2;
	uint8_t init:1;
}comHandlerStatusNConfig_t;

extern uint8_t initUserUnit(usartConfig_t config);
extern uint8_t usartDataTx(uint8_t* data, uint16_t length, uint32_t timeout_us);
extern uint8_t usartDataRx(uint8_t* data, uint16_t length, uint32_t timeout_us);

#endif /* USERUNIT_H_ */