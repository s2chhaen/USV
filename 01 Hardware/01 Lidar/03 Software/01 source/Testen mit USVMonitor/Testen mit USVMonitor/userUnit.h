/*
 * userUnit.h
 *
 * Created: 7/10/2023 4:48:20 AM
 * Author: Thach
 */ 


#ifndef USERUNIT_H_
#define USERUNIT_H_

#include <stdint.h>
#include <avr/io.h>
#include "errorList.h"
#include "ATMegaXX09/USART/USART.h"

#define RX_BUFFER_LEN 100
#define TX_BUFFER_LEN 100
#define NO_OF_RX_BUFFER 2
#define MAX_BYTE_SEND 31

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
	uint8_t rxByte:5;
}usartConfig_t;

typedef struct{
	struct rxUnit{
		volatile uint8_t rxBuffer[NO_OF_RX_BUFFER][RX_BUFFER_LEN];
		volatile uint16_t toRxByte[NO_OF_RX_BUFFER];
		volatile uint16_t strReadPtr;
		const uint16_t rxLenMax;
		const uint8_t fifoLenMax;
		volatile uint8_t readFIFOPtr;
		volatile uint8_t writeFIFOPtr;
	}rxObj;
	struct txUnit{
		volatile uint8_t txBuffer[TX_BUFFER_LEN];
		volatile uint16_t toTxByte;
		volatile uint16_t strReadPtr;
		const uint16_t txLenMax;
		
	}txObj;
	struct status{
		volatile uint8_t usart:2;
		uint8_t initState:1;
		uint8_t crcActive:1;
		volatile uint8_t rxBufferState:2;//Leer, Belegt, Voll
	}statusObj;
}userUnit_t;

extern uint8_t initUserUnit(usartConfig_t config);
extern uint8_t usartDataTx(uint8_t* data, uint16_t length);
extern uint8_t usartDataRx(uint8_t* data, uint16_t* length);





#endif /* USERUNIT_H_ */