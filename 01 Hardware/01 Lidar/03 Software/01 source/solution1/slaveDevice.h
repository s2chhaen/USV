/*
 * slave.h
 * Für die Slave-Gerät
 * Created: 21.06.2023 12:38:43
 * Author: hathach
 * Version: 0.1
 */ 


#ifndef SLAVE_H_
#define SLAVE_H_

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <avr/interrupt.h>
#include "lock.h"
#include "error_list.h"
#include "ATMegaXX09/USART/USART.h"
 
 //#define SLAVE_STATIC 0

#define MAX_BYTE_SEND 31
//#define VERSION_1

#ifdef SLAVE_DYNAMIC
typedef struct {
	uint8_t uart;
	volatile uint16_t txTime;
	volatile uint16_t toTxByte;
	volatile uint16_t rxTime;
	volatile uint16_t toRxByte;
	
	uint16_t txLenMax;
	volatile uint8_t *txBuffer;
	volatile uint16_t txPtrPosition;
	uint16_t rxLenMax;
	volatile uint8_t *rxBuffer;
	//volatile uint16_t rxPtrPosition;
	uint8_t initState:1;
	uint8_t crcActive:1;
}slaveDevice_t;
#else
typedef struct {
	uint8_t uart;
	volatile uint16_t txTime;
	volatile uint16_t toTxByte;
	volatile uint16_t rxTime;
	volatile uint16_t toRxByte;
	
	uint16_t txLenMax;
	volatile uint8_t txBuffer[900];
	volatile uint16_t txPtrPosition;
	uint16_t rxLenMax;
	volatile uint8_t rxBuffer[900];
	volatile uint16_t rxPtrPosition;
	uint8_t initState:1;
	uint8_t crcActive:1;
}slaveDevice_t;
#endif

typedef enum {
	ON,
	OFF
}state_t;

extern processResult_t initDev(uint16_t rxLength, uint16_t txLength,uint8_t USARTnumber, uint32_t baudrate, \
					USART_CHSIZE_t bits, USART_PMODE_t parity,USART_SBMODE_t stopbit, \
					bool sync, bool MPCM, uint8_t address, PORTMUX_USARTx_t PortMux);

extern processResult_t dataTx(uint8_t* data, uint16_t length);
extern processResult_t dataRx(uint16_t length, uint8_t* data);

#endif /* SLAVE_H_ */