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
#include <string.h>
#include "lock.h"
#include "error_list.h"
#include "ATMegaXX09/USART/USART.h"
#include "slaveDeviceConfig.h"
 

#define MAX_BYTE_SEND 31
//#define VERSION_1

//Refaktorisierung in Bearbeitung

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
#endif
#ifndef FULL_STRUCT
typedef struct {
	uint8_t uart;
	volatile uint16_t txTime;
	volatile uint16_t toTxByte;
	volatile uint16_t rxTime;
	volatile uint16_t toRxByte;
	
	uint16_t txLenMax;
	volatile uint8_t txBuffer[TX_BUFFER_LEN];
	volatile uint16_t txPtrPosition;
	uint16_t rxLenMax;
	volatile uint8_t rxBuffer[RX_BUFFER_LEN];
	volatile uint16_t rxPtrPosition;
	uint8_t initState:1;
	uint8_t crcActive:1;
}slaveDevice_t;
#endif

#ifdef FULL_STRUCT

typedef processResult_t (*txRxData_t)(uint8_t*, uint16_t);

typedef struct {
	txRxData_t dataTx_p;
	txRxData_t dataRx_p;
	struct rxUnit{
		volatile uint8_t rxBuffer[NO_OF_RX_BUFFER][RX_BUFFER_LEN];
		volatile uint16_t toRxByte;
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
		uint8_t uart:2;
		uint8_t initState:1;
		uint8_t crcActive:1;
		uint8_t rxBufferState:2;
		uint8_t msgAvaliable:1;
	}statusObj;
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
extern processResult_t dataRx(uint8_t* data, uint16_t length);

#endif /* SLAVE_H_ */