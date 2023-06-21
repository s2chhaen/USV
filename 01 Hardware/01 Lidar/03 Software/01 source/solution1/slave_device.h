/*
 * slave.h
 * Für die Slave-Gerät
 * Created: 21.06.2023 12:38:43
 *  Author: hathach
 */ 


#ifndef SLAVE_H_
#define SLAVE_H_

#include "error_list.h"
#include "ATMegaXX09/USART/USART.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
 
typedef struct {
	uint8_t uart;
	volatile uint16_t txTime;
	//volatile uint16_t toTxByte;
	volatile uint16_t rxTime;
	//volatile uint16_t toRxByte;
	uint16_t rxLenMax;
	uint16_t txLenMax;
	volatile uint8_t *txBuffer;
	volatile uint8_t *rxBuffer;
	uint8_t initState:1;
	uint8_t crcActive:1;
}slaveDevice_t;

typedef enum {
	ON,
	OFF
}state_t;

extern processResult_t initDev(uint16_t rxLength, uint16_t txLength,uint8_t USARTnumber, uint32_t baudrate, \
					USART_CHSIZE_t bits, USART_PMODE_t parity,USART_SBMODE_t stopbit, \
					bool sync, bool MPCM, uint8_t address, PORTMUX_USARTx_t PortMux);

extern processResult_t dataTx(uint8_t length, uint8_t* data);
extern uint8_t DataRx(uint8_t length, uint8_t* data);

extern bool callbackRx(uint8_t adress, uint8_t data[], uint8_t length);
extern bool callbackTx(uint8_t* adress, uint8_t* data[], uint8_t* length,uint8_t max_length);


#endif /* SLAVE_H_ */