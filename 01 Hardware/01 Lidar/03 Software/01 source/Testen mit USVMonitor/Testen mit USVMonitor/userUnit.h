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
#include <string.h>
#include "errorList.h"
#include "ATMegaXX09/USART/USART.h"
#include "ATMegaXX09/FIFO/FIFO.h"
#include "Math/MinMax.h"

#define RX_BUFFER_LEN 50
#define TX_BUFFER_LEN 50
#define NO_OF_RX_BUFFER 1
#define BYTE_RECEIVE_TIME_US 100
#define NO_OF_TX_BUFFER 1
#define MAX_USART_FIFO 31

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
		volatile uint8_t rxBuffer[RX_BUFFER_LEN];
		volatile uint16_t toRxByte;
		volatile uint16_t strPtr;
		const uint16_t rxLenMax;
		const uint16_t usartFIFOMax;
	}rxObj;
	struct txUnit{
		volatile uint8_t txBuffer[TX_BUFFER_LEN];
		volatile int16_t toTxByte;
		volatile uint16_t strPtr;
		const uint16_t txLenMax;
		const uint16_t usartFIFOMax;
	}txObj;
	struct status{
		volatile uint8_t usart:2;
		uint8_t initState:1;
	}statusObj;
}userUnit_t;

extern uint8_t initUserUnit(usartConfig_t config);
extern uint8_t usartDataTx(uint8_t* data, uint16_t length);
extern uint8_t usartDataRx(uint8_t* data, uint8_t length);





#endif /* USERUNIT_H_ */